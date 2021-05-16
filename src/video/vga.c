#include "vga.h"
#include "io/io.h"
#include "kernel.h"
//#include "font8.h"

char* VGA_Address = 0x00;

int VGA_GUI_WIDTH = 320;
int VGA_GUI_HEIGHT = 200;


unsigned char g_640x480x16[] =
{
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

static uint8_t mode_320_200_256[]={
   /* MISC 
    *
    * 0x63 => 01100011
    * 7 6 5 4 3 2 1 0
    * 1 1 0 0 0 1 1 0
    * VSP HSP - - CS CS ERAM IOS
    * 7,6 - 480 lines
    * 5,4 - free
    * 3,2 - 28,322 MHZ Clock
    * 1 - Enable Ram
    * 0 - Map 0x3d4 to 0x3b4
    */
   0x63,
   /* SEQ */
   /**
    * index 0x00 - Reset
    * 0x03 = 11
    * Bits 1,0 Synchronous reset
    */
   0x03,
   /**
    * index 0x01
    * Clocking mode register
    * 8/9 Dot Clocks
    */
   0x01,
   /**
    * Map Mask Register, 0x02
    * 0x0F = 1111
    * Enable all 4 Maps Bits 0-3
    * chain 4 mode
    */
   0x0F,
   /**
    * map select register, 0x03
    * no character map enabled
    */
   0x00,
   /**
    * memory mode register 0x04
    * enables ch4,odd/even,extended memory
    */
   0x0E,
   /* CRTC */
   0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
   0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x9C, 0x0E, 0x8F, 0x28,   0x40, 0x96, 0xB9, 0xA3,
   0xFF,
   /* GC */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
   0xFF,
   /* AC */
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
   0x41, 0x00, 0x0F, 0x00,   0x00
};

void write_registers(uint8_t* regs){
   unsigned i;

   /* write MISCELLANEOUS reg */
   outb(VGA_MISC_WRITE, *regs);
   regs++;
   /* write SEQUENCER regs */
   for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
   {
      outb(VGA_SEQ_INDEX, i);
      outb(VGA_SEQ_DATA, *regs);
      regs++;
   }
   /* unlock CRTC registers */
   outb(VGA_CRTC_INDEX, 0x03);
   outb(VGA_CRTC_DATA, insb(VGA_CRTC_DATA) | 0x80);
   outb(VGA_CRTC_INDEX, 0x11);
   outb(VGA_CRTC_DATA, insb(VGA_CRTC_DATA) & ~0x80);
   /* make sure they remain unlocked */
   regs[0x03] |= 0x80;
   regs[0x11] &= ~0x80;
   /* write CRTC regs */
   for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
   {
      outb(VGA_CRTC_INDEX, i);
      outb(VGA_CRTC_DATA, *regs);
      regs++;
   }
   /* write GRAPHICS CONTROLLER regs */
   for(i = 0; i < VGA_NUM_GC_REGS; i++)
   {
      outb(VGA_GC_INDEX, i);
      outb(VGA_GC_DATA, *regs);
      regs++;
   }
   /* write ATTRIBUTE CONTROLLER regs */
   for(i = 0; i < VGA_NUM_AC_REGS; i++)
   {
      (void)insb(VGA_INSTAT_READ);
      outb(VGA_AC_INDEX, i);
      outb(VGA_AC_WRITE, *regs);
      regs++;
   }
   
   /* lock 16-color palette and unblank display */
   (void)insb(VGA_INSTAT_READ);
   outb(VGA_AC_INDEX, 0x20);
}

void set_plane(uint8_t p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;
/* set read plane */
	outb(VGA_GC_INDEX, 4);
	outb(VGA_GC_DATA, p);
/* set write plane */
	outb(VGA_SEQ_INDEX, 2);
	outb(VGA_SEQ_DATA, pmask);
}

uint16_t get_fb_seg()
{
	uint16_t seg;

	outb(VGA_GC_INDEX, 6);
	seg = insw(VGA_GC_DATA);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}

uint8_t get_color(uint8_t r, uint8_t g, uint8_t b)
{
    return (r*6/256)*36 + (g*6/256)*6 + (b*6/256);
}

void fill_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b)
{
    for(int32_t Y = y; Y < y+h; Y++)
        for(int32_t X = x; X < x+w; X++)
            VGA_Address[VGA_GUI_WIDTH*Y+X] = get_color(r,g,b);
}

void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color)
{
    VGA_Address[VGA_GUI_WIDTH*y+x] = color;
}

void vga_putchar(uint32_t x, uint32_t y, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b,
    uint8_t bg_r, uint8_t bg_g, uint8_t bg_b, int font_size, const char c) 
{
    // int i, j;
    // for(i = 0; i < 8; i++)
    //     for(j = 0; j < 8; j++) {
    //         uint8_t fontChar = ((font8x8_basic[c & 0x7F][j] >> i ) & 1);
    //         if (fontChar == 0x00)
    //             vga_put_pixel( x+i, y+j, 0x00 );
    //         else
    //             vga_put_pixel(x+i,y+j, 0x3F);
    //     }
}

void vga_clear_screen(){
   unsigned int x=0;
   unsigned int y=0;

   for(y=0; y< VGA_GUI_HEIGHT; y++){
      for(x=0; x< VGA_GUI_WIDTH; x++){
         VGA_Address[VGA_GUI_WIDTH*y+x]=0x00;
      }
   }
}

void vga_init(){
    //setup the vga struct
    VGA_Address = (char*)0xA0000;

    //enables the mode 13 state
    int i = 0;
    if (i == 0)
    {
        write_registers(g_640x480x16);
        VGA_GUI_WIDTH = 640;
        VGA_GUI_HEIGHT = 480;
    }
    else 
    {
        write_registers(mode_320_200_256);
        VGA_GUI_WIDTH = 320;
        VGA_GUI_HEIGHT = 200;
    }

}
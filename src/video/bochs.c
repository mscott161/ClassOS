#include "bochs.h"
#include "font8.h"
#include "kernel.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "pci/pci.h"

uint8_t bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function;

uint32_t scanline = 0;
uint32_t pixel_size = 0;

uint16_t w, h, d;

uint8_t* bochs_vbe_fb = 0;
uint8_t bochs_vbe_fb_current = 0;
uint32_t* bochs_vbe_fb_double[2];

static uint32_t  update_x1, update_x0, update_y1, update_y0;
// expand ploted area
#define BOCHS_AREA_GROW(x,y)  { \
                            if( x > update_x1 ) update_x1 = x; \
                            if( x < update_x0 ) update_x0 = x; \
                            if( y > update_y1 ) update_y1 = y; \
                            if( y < update_y0 ) update_y0 = y; \
                            }

void bochs_vbe_write(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

uint16_t bochs_vbe_read(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return insw(VBE_DISPI_IOPORT_DATA);
}

void bochs_vbe_disable() {
    bochs_vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

void bochs_vbe_enable() {
    bochs_vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void bochs_init(uint16_t width, uint16_t height, uint16_t depth)
{
    bochs_vbe_fb = pci_get_lfb();
    if (!bochs_vbe_fb)
    {
      printf2("Bad Memory\n");
      return;
    }

    bochs_vbe_fb_current = 0;
    bochs_vbe_fb_double[0] = (uint32_t*)bochs_vbe_fb;
    bochs_vbe_fb_double[1] = (uint32_t*)bochs_vbe_fb; // set correct address when set video mode->

    scanline = width * 4;
    pixel_size = 4; // 24-bit colors

    w = width;
    h = height;
    d = depth;

    update_x0 = update_y0 = 0xFFFFFFFF;
    update_x1 = update_y1 = 0x0;

    bochs_vbe_fb_double[1] = (uint32_t*)(bochs_vbe_fb + w * h * (d >> 3));

    bochs_vbe_disable();
    bochs_vbe_write(VBE_DISPI_INDEX_XRES, width);
    bochs_vbe_write(VBE_DISPI_INDEX_YRES, height);
    bochs_vbe_write(VBE_DISPI_INDEX_BPP, depth);
    // bochs_vbe_write(VBE_DISPI_INDEX_Y_OFFSET, 0);
    // bochs_vbe_write(VBE_DISPI_INDEX_X_OFFSET, 0);
    bochs_vbe_enable();
    printf2("End Init\n");
}

void bochs_vbe_putpixel_32(uint32_t x, uint32_t y, color_t c) {
    uint32_t offset;
    if( x > w || y > h)
        return;
    offset = (y * w) + (x);
    bochs_vbe_fb_double[bochs_vbe_fb_current][offset] = c.value;
}

void bochs_vbe_putchar(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c) {
    uint8_t i, j;
    for(j = 0; j < 8; j++) 
      for(i = 0; i < 8; i++) {
        bochs_vbe_putpixel_32( x+i, y+j, ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? fgcolor : bgcolor );
      }
}

void bochs_vbe_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t color) {
	uint32_t x, y, temp;

  if(y0 > y1) {
    temp = y0;
    y0 = y1;
    y1 = temp;
  }
  if(x0 > x1) {
    temp = x0;
    x0 = x1;
    x1 = temp;
  }

  for( y = y0; y < y1; y++ )
    for( x = x0; x < x1; x++ )
        bochs_vbe_putpixel_32(x,y,color);
  
}

void bochs_vbe_flip() 
{

  if( update_x1 > 0 ) { // has something to plot ?
    bochs_vbe_write(VBE_DISPI_INDEX_Y_OFFSET, h * bochs_vbe_fb_current);


    bochs_vbe_fb_current = (bochs_vbe_fb_current == 0);

    update_x0 = update_y0 = 0xFFFFFFFF;
    update_x1 = update_y1 = 0x0;
  }

}

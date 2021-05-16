#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

//#define VGA_GUI_WIDTH 320
//#define VGA_GUI_HEIGHT 200

#define VGA_AC_INDEX 0x3C0
#define VGA_AC_WRITE 0x3C0
#define VGA_AC_READ 0x3C1
#define VGA_MISC_WRITE 0x3C2
#define VGA_SEQ_INDEX 0x3C4
#define VGA_SEQ_DATA 0x3C5
#define VGA_DAC_READ_INDEX 0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA 0x3C9
#define VGA_MISC_READ 0x3CC
#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF
#define VGA_CRTC_INDEX 0x3D4      /* 0x3B4 */
#define VGA_CRTC_DATA 0x3D5      /* 0x3B5 */
#define VGA_INSTAT_READ 0x3DA
#define VGA_NUM_SEQ_REGS 5
#define VGA_NUM_CRTC_REGS 25
#define VGA_NUM_GC_REGS 9
#define VGA_NUM_AC_REGS 21
#define VGA_NUM_REGS (1+VGA_NUM_SEQ_REGS+VGA_NUM_CRTC_REGS+VGA_NUM_GC_REGS+VGA_NUM_AC_REGS)

void vga_init();
void vga_clear_screen();
void fill_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);
void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color);
void vga_putchar(uint32_t x, uint32_t y, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b,
    uint8_t bg_r, uint8_t bg_g, uint8_t bg_b, int font_size, const char c);

#endif

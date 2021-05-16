#ifndef BOCHS_H
#define BOCHS_H

#include <stdint.h>

#define PCI_VENDOR_DEVICE 0x00
#define PCI_CLASS_SUBCLASS 0x08
#define PCI_CONFIG 0xCF8
#define PCI_DATA 0xCFC

#define BOCHS_VBE_VENDOR  0x1111
#define BOCHS_VBE_DEVICE  0x1234

#define VBE_DISPI_IOPORT_INDEX  0x1CE
#define VBE_DISPI_IOPORT_DATA   0x1CF

#define VBE_DISPI_INDEX_ID          0
#define VBE_DISPI_INDEX_XRES        1
#define VBE_DISPI_INDEX_YRES        2
#define VBE_DISPI_INDEX_BPP         3
#define VBE_DISPI_INDEX_ENABLE      4
#define VBE_DISPI_INDEX_BANK        5
#define VBE_DISPI_INDEX_VIRT_WIDTH  6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET    8
#define VBE_DISPI_INDEX_Y_OFFSET    9

// flags for VBE_DISPI_INDEX_ENABLE
#define VBE_DISPI_DISABLED          0x00
#define VBE_DISPI_ENABLED           0x01
#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

struct device_descriptor
{
  uint32_t portBase;
  uint32_t interrupt;

  uint16_t bus;
  uint16_t device;
  uint16_t function;

  uint16_t vendor_id;
  uint16_t device_id;

  uint8_t class_id;
  uint8_t subclass_id;
  uint8_t interface_id;

  uint8_t revision;
};

typedef union color {
  uint32_t  value;
//   struct {
//     uint8_t a, r, g, b;
//   };
} color_t;


void bochs_init(uint16_t width, uint16_t height, uint16_t depth);
void bochs_vbe_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t color);
void bochs_vbe_putchar(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c);
void bochs_vbe_flip();

#endif


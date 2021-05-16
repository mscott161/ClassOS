#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define MAX_PATH 108

#include <stdint.h>

void kernel_main();
void print(const char* str);
void printfHex(uint8_t key);
void printfHex16(uint16_t key);
void printfHex32(uint32_t key);
void printf2(const char* fmt, ...);
void terminal_writechar(char c, char color);
void panic(const char* msg);
void kernel_page();
void kernel_registers();
void terminal_putchar(int x, int y, char c, char color);

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif
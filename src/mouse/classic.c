#include "classic.h"
#include "mouse.h"
#include "io/io.h"
#include "kernel.h"
#include "idt/idt.h"
#include "task/task.h"

#include <stdint.h>
#include <stddef.h>

int classic_mouse_init();

struct mouse class_mouse = {
    .name = { "Classic" },
    .init = classic_mouse_init
};

void classic_mouse_handle_interrupt();

typedef enum MouseCommands
{
    MOUSE_ACK = 0xFA,
    MOUSE_IDENTITY = 0xF2,
    MOUSE_ENABLE_PACKETS = 0xF4,
    MOUSE_DISABLE_DATA = 0xF5,
    MOUSE_READ_PORT = 0x60,
    MOUSE_WRITE_PORT = 0x64,
} MouseCommands;

uint8_t offset;
uint8_t buffer[3];
uint8_t mouse_key;
int8_t mouse_x = 40, mouse_y = 12;

int mouseX = 40;
int mouseY = 12;

void OnMouseMove(int xoffset, int yoffset)
{
    terminal_putchar(mouseX, mouseY, 32, 0);

    mouseX += xoffset;
    if(mouseX >= VGA_WIDTH) 
        mouseX = VGA_WIDTH - 1;
    if(mouseX <= 0) 
        mouseX = 0;

    mouseY += yoffset;
    if(mouseY >= 25) 
        mouseY = 25 - 1;
    if(mouseY <= 0) 
        mouseY = 0;

    terminal_putchar(mouseX, mouseY, 219, 15);
}

int classic_mouse_init()
{
    idt_register_interrupt_callback(0x2C, classic_mouse_handle_interrupt);

    outb(0x64, 0xA8);
    
    outb(0x64, 0x20);
    uint8_t status = insb(0x60) | 2;
    outb(0x64, 0x60);
    outb(0x60, status);
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    insb(0x60);

print("Mouse Init\n");
  return 0;
}

void classic_mouse_handle_interrupt()
{
    outb(0xA0, 0x20); // PIC Ack Slave
    
    uint8_t __status = insb(0x64);
    if (!(__status & 0x20))
        return;

    buffer[offset] = insb(0x60);
    offset = (offset + 1) % 3;

    if(offset == 0)
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
            OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }

        for(uint8_t i = 0; i < 3; i++)
        {
            // if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            // {
            //     if(buttons & (0x1<<i))
            //         handler->OnMouseUp(i+1);
            //     else
            //         handler->OnMouseDown(i+1);
            // }
        }
        // buttons = buffer[0];
    }
}

struct mouse* classic_mice_init()
{
    return &class_mouse;
}

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

void mouse_wait(uint8_t a_type) //unsigned char
{
  uint32_t _time_out=100000; //unsigned int

  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((insb(0x64) & 1)==1)
      {
        print("Type 0 Return\n");
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((insb(0x64) & 2)==0)
      {
        print("Type 1 Return\n");
        return;
      }
    }
    return;
  }
}

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

  outb(PS2_MOUSE_PORT, 0xA8);
  outb(PS2_MOUSE_PORT, 0x20);
  
  uint8_t status = insb(MOUSE_INPUT_PORT) | 2;
  outb(PS2_MOUSE_PORT, 0x60);
  outb(MOUSE_INPUT_PORT, status);

  outb(PS2_MOUSE_PORT, 0xD4);
  outb(MOUSE_INPUT_PORT, 0xF4);
  insb(MOUSE_INPUT_PORT);

  uint8_t buffer[3];
  uint8_t offset = 0;
  while(1)
  {
        uint8_t status = insb(PS2_MOUSE_PORT);

        if (!(status & 0x20))
            continue;

        buffer[offset] = insb(MOUSE_INPUT_PORT);
                
        offset = (offset + 1) % 3;

        if(offset == 0)
        {
            if(buffer[1] != 0 || buffer[2] != 0)
            {
              //printf2("Move %i %i ", (int8_t)buffer[1], -((int8_t)buffer[2]));
              OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
            }
        }
  }
  print("END OF MOUSE\n");

    return 0;
}

void classic_mouse_handle_interrupt()
{
    print("Mouse Handler\n");
    // kernel_page();

    // mouse_data(0, insb(0x60));
    // mouse_data(1, insb(0x60));
    // mouse_data(2, insb(0x60));

    // task_page();
}

struct mouse* classic_mice_init()
{
    return &class_mouse;
}

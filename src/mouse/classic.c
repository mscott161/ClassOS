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
        return;
      }
    }
    return;
  }
}

int classic_mouse_init()
{

    uint8_t _status;  //unsigned char

    //Enable the auxiliary mouse device
    mouse_wait(1);
    outb(PS2_MOUSE_PORT, PS2_MOUSE_COMMAND_ENABLE_SECOND_PORT);
    
    //Enable the interrupts
    mouse_wait(1);
    outb(PS2_MOUSE_PORT, 0x20);
    mouse_wait(0);
    _status=(insb(0x60) | 2);
    //_status = insb(0x60);
    //_status |= 1 << 1;
    mouse_wait(1);
    outb(PS2_MOUSE_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_INPUT_PORT, _status);
    
    //Tell the mouse to use default settings
    //Tell the mouse we are sending a command
    //Wait to be able to send a command
    mouse_wait(1);
    outb(PS2_MOUSE_PORT, 0xD4);

    //Wait for the final part
    mouse_wait(1);
    outb(MOUSE_INPUT_PORT, 0xF6);
    mouse_wait(0);
    insb(MOUSE_INPUT_PORT);  //Acknowledge
    
    //Tell the mouse we are sending a command
    mouse_wait(1);
    outb(PS2_MOUSE_PORT, 0xD4);
    //Enable the mouse
    mouse_wait(1);
    outb(MOUSE_INPUT_PORT, 0xF4);
    mouse_wait(0);
    insb(MOUSE_INPUT_PORT);  //Acknowledge

    //Setup the mouse handler
    idt_register_interrupt_callback(0x2C, classic_mouse_handle_interrupt);

    return 0;
}

void classic_mouse_handle_interrupt()
{
    print("Mouse Handler\n");
    kernel_page();

    mouse_data(0, insb(0x60));
    mouse_data(1, insb(0x60));
    mouse_data(2, insb(0x60));

    task_page();
}

struct mouse* classic_mice_init()
{
    return &class_mouse;
}

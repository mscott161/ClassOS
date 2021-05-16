#ifndef CLASSIC_MOUSE_H
#define CLASSIC_MOUSE_H

#define PS2_MOUSE_PORT 0x64
#define PS2_MOUSE_COMMAND_ENABLE_SECOND_PORT 0xA8

#define ISR_MOUSE_INTERRUPT 0x2C
#define MOUSE_INPUT_PORT 0x60

struct mouse* classic_mice_init();

#endif

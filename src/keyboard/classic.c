#include "keyboard.h"
#include "classic.h"
#include "io/io.h"
#include "kernel.h"
#include "idt/idt.h"
#include "task/task.h"
#include "string/string.h"

#include <stdint.h>
#include <stddef.h>

#define CLASSIC_KEYBOARD_CAPSLOCK 0x3A
#define CLASSIC_KEYBOARD_RIGHT_SHIFT 0x36
#define CLASSIC_KEYBOARD_LEFT_SHIFT 0x2A

int classic_keyboard_init();

static uint8_t keyboard_scan_set_one[] ={
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0D, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`',
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

struct keyboard classic_keyboard = {
    .name = { "Classic" },
    .init = classic_keyboard_init
};

void classic_keyboard_handle_interrupt();

int classic_keyboard_init()
{
    idt_register_interrupt_callback(ISR_KEYBOARD_INTERRUPT, classic_keyboard_handle_interrupt);

    keyboard_set_capslock(&classic_keyboard, KEYBOARD_CAPS_LOCK_OFF);
    // keyboard_set_right_shift(&classic_keyboard, KEYBOARD_SHIFT_RIGHT_UP);
    // keyboard_set_left_shift(&classic_keyboard, KEYBOARD_SHIFT_LEFT_UP);

    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);

    return 0;
}

uint8_t class_keyboard_scancode_to_char(uint8_t scancode)
{
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if (scancode > size_of_keyboard_set_one)
    {
        return 0;
    }

    char c = keyboard_scan_set_one[scancode];

    if (keyboard_get_capslock(&classic_keyboard) == KEYBOARD_CAPS_LOCK_OFF)
    {
        if (c >= 'A' && c <= 'Z')
            c += 32;
    }

    // if (keyboard_get_right_shift(&classic_keyboard) == KEYBOARD_SHIFT_RIGHT_DOWN)
    // {
    //     if (c >= 'A' && c <= 'Z')
    //     {
    //         if (keyboard_get_capslock(&classic_keyboard) == KEYBOARD_CAPS_LOCK_ON)
    //             c -= 32;
    //         else
    //             c += 32;
    //     }
    // }

    // if (keyboard_get_left_shift(&classic_keyboard) == KEYBOARD_SHIFT_LEFT_DOWN)
    // {
    //     if (c >= 'A' && c <= 'Z')
    //     {
    //         if (keyboard_get_capslock(&classic_keyboard) == KEYBOARD_CAPS_LOCK_ON)
    //             c -= 32;
    //         else
    //             c += 32;
    //     }
    // }
    
    return c;
}

void classic_keyboard_handle_interrupt()
{
    print("classic_keyboard\n");
    kernel_page();
    uint8_t scancode = 0;
    scancode = insb(KEYBOARD_INPUT_PORT);
    insb(KEYBOARD_INPUT_PORT);

    if (scancode & CLASSIC_KEYBOARD_KEY_RELEASED)
    {
        return;
    }

    if (scancode == CLASSIC_KEYBOARD_CAPSLOCK)
    {
        KEYBOARD_CAPS_LOCK_STATE old_state = keyboard_get_capslock(&classic_keyboard);
        keyboard_set_capslock(&classic_keyboard, old_state == KEYBOARD_CAPS_LOCK_ON ? KEYBOARD_CAPS_LOCK_OFF : KEYBOARD_CAPS_LOCK_ON);
    }

    // if (scancode == CLASSIC_KEYBOARD_RIGHT_SHIFT)
    // {
    //     KEYBOARD_SHIFT_RIGHT_STATE old_right_shift_state = keyboard_get_right_shift(&classic_keyboard);
    //     keyboard_set_right_shift(&classic_keyboard, old_right_shift_state == KEYBOARD_SHIFT_RIGHT_DOWN ? KEYBOARD_SHIFT_RIGHT_UP : KEYBOARD_SHIFT_RIGHT_DOWN);
    //     print("right_shift_pressed\n");
    //     return;
    // }

    // if (scancode == CLASSIC_KEYBOARD_LEFT_SHIFT)
    // {
    //     KEYBOARD_SHIFT_LEFT_STATE old_left_shift_state = keyboard_get_left_shift(&classic_keyboard);
    //     keyboard_set_left_shift(&classic_keyboard, old_left_shift_state == KEYBOARD_SHIFT_LEFT_DOWN ? KEYBOARD_SHIFT_LEFT_UP : KEYBOARD_SHIFT_LEFT_DOWN);
    //     print("left_shift_pressed\n");
    //     return;
    // }

    uint8_t c = class_keyboard_scancode_to_char(scancode);

    if (c != 0)
    {
        keyboard_push(c);
    }

    task_page();
}

struct keyboard* classic_key_init()
{
    return &classic_keyboard;
}
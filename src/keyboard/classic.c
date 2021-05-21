#include "keyboard.h"
#include "classic.h"
#include "io/io.h"
#include "kernel.h"
#include "idt/idt.h"
#include "task/task.h"
#include "string/string.h"

#include "pic/pic.h"

#include <stdint.h>
#include <stddef.h>

#define CLASSIC_KEYBOARD_CAPSLOCK 0x3A

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

	// // Get the current scancode set
	// ps2_write_device(0, 0xF0);
	// ps2_expect_ack();
	// ps2_write_device(0, 0x00);
	// ps2_expect_ack();
	// uint8_t scancode_set = ps2_read(PS2_DATA);

	// if (scancode_set != 0x02) {
	// 	printf2("[KBD] Wrong scancode set (%d), TODO\n", scancode_set);
	// }

	// // Start receiving IRQ1s
	// ps2_write_device(0, PS2_DEV_ENABLE_SCAN);
	// ps2_expect_ack();

    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    // while(insb(PS2_PORT) & 0x1)
    //     insb(KEYBOARD_INPUT_PORT);
    // outb(PS2_PORT, 0xae); // activate interrupts
    // outb(PS2_PORT, 0x20); // command 0x20 = read controller command byte

    // //uint8_t status = (insb(KEYBOARD_INPUT_PORT) | 1) & ~0x10;
    // uint8_t status = insb(KEYBOARD_INPUT_PORT);
    // printf2("Key [%i]\n", status);
    // printf2("Key [%i]\n", (status | 1));
    // printf2("Key [%i]\n", (status | 1) & ~0x10);
    
    // status = (status | 1) & ~0x10;

    
    
    // outb(PS2_PORT, 0x60); // command 0x60 = set controller command byte
    // outb(KEYBOARD_INPUT_PORT, status);
    // outb(KEYBOARD_INPUT_PORT, 0xf4);
print("Keyboard Init\n");
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
    
    return c;
}

void classic_keyboard_handle_interrupt()
{
    outb(0x20, 0x20); // PIC Ack Master

    kernel_page();
    uint8_t scancode = 0;
    scancode = insb(KEYBOARD_INPUT_PORT);

    if (scancode & CLASSIC_KEYBOARD_KEY_RELEASED)
    {
        task_page();
        return;
    }

    if (scancode == CLASSIC_KEYBOARD_CAPSLOCK)
    {
        KEYBOARD_CAPS_LOCK_STATE old_state = keyboard_get_capslock(&classic_keyboard);
        keyboard_set_capslock(&classic_keyboard, old_state == KEYBOARD_CAPS_LOCK_ON ? KEYBOARD_CAPS_LOCK_OFF : KEYBOARD_CAPS_LOCK_ON);
    }

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
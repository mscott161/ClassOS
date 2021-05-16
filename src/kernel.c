#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"

#include "task/task.h"
#include "task/process.h"

#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"
#include "mouse/mouse.h"

#include "status.h"

#include "pci/pci.h"
#include "video/bochs.h"

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_backspace()
{
    if (terminal_row == 0 && terminal_col == 0)
    {
        return;
    }

    if (terminal_col == 0)
    {
        terminal_row -= 1;
        terminal_col = VGA_WIDTH;
    }

    terminal_col -= 1;
    terminal_writechar(' ', 15);

    if (terminal_col == 0)
    {
        terminal_row -= 1;
        terminal_col = VGA_WIDTH - 1;
    }
    else
    {
        terminal_col -= 1;
    }
}

void terminal_writechar(char c, char color)
{
    if (c == '\n')
    {
        terminal_row++;
        terminal_col = 0;
        return;
    }

    if (c == 0x08)
    {
        terminal_backspace();
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row++;
    }

    if (terminal_row >= 25)
    {
        terminal_col = 0;
        terminal_row = 0;
    }
}

void terminal_initialize()
{
    video_mem = (uint16_t*)0xB8000;
    terminal_row = 0;
    terminal_col = 0;

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

char* itoa(int i)
{
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;
    if (i >= 0)
    {
        neg = 0;
        i = -i;
    }

    while (i)
    {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11)
        text[--loc] = '0';
    
    if (neg)
        text[--loc] = '-';
    
    return &text[loc];
}


void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    print(foo);
}
void printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint32_t key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}

void printf2(const char* fmt, ...)
{
    va_list ap;
    const char* p;
    char* sval;
    int ival;

    va_start(ap, fmt);

    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            terminal_writechar(*p, 15);
            continue;
        }

        switch(*++p)
        {
            case 'i': // %i
                ival = va_arg(ap, int);
                print(itoa(ival));
                break;

            case 'x': // %x
                ival = va_arg(ap, int);
                char* foo = "00";
                char* hex = "0123456789ABCDEF";
                foo[0] = hex[(ival >> 4) & 0xF];
                foo[1] = hex[ival & 0xF];
                terminal_writechar(foo[0], 15);
                terminal_writechar(foo[1], 15);
                break;

            case 's': // %s
                sval = va_arg(ap, char*);
                print(sval);
                break;

            default:
                terminal_writechar(*p, 15);
                break;
        }
    }

    va_end(ap);
}

static struct paging_4gb_chunk* kernel_chunk = 0;

void panic(const char* msg)
{
    print(msg);
    while(1) {}
}

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;

struct gdt gdt_real[TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},   // NULL Segments
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A}, // Kernel Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92}, // Kernel Data Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF8}, // User Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF2}, // User Data Segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9} // TSS Segment
};

void kernel_main()
{
    terminal_initialize();

    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, TOTAL_GDT_SEGMENTS);

    // Load the GDT
    gdt_load(gdt_real, sizeof(gdt_real));

    // Initialize the heap
    kheap_init();

    // Initialize File Systems
    fs_init();

    // Search and initialize the disk
    disk_search_and_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // Setup the TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28);

    // Setup Paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(kernel_chunk);

    // Enable Paging
    enable_paging();

    // Register the kernel commands
    isr80h_register_commands();

    // Initialize PCI Controller
    //pci_init();

    // Initialize all the system keyboards
    keyboard_init();

    // Initialize all the system mice
    //mouse_init();

    // Initialize Bochs VGA
    //bochs_init(640, 480, 32);

    //bochs_vbe_rect(0, 0, 320, 240, (color_t){0xC9C9C9});

    struct process* process = 0;
    int res = process_load_switch("0:/shell.elf", &process);
    if (res != ALL_OK)
    {
        panic("Failed to load shell.elf\n");
    }

    struct command_argument argument;
    strcpy(argument.argument, "Testing!");
    argument.next = 0x00;

    process_inject_arguments(process, &argument);

    task_run_first_ever_task();

    while(1) {
        
    }
}


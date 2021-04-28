#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

struct process;

typedef int (*MOUSE_INIT_FUNCTION)();

struct mouse
{
    MOUSE_INIT_FUNCTION init;
    char name[20];
    struct mouse* next;
};

void mouse_init();
int mouse_insert(struct mouse* mouse);
void mouse_data(uint8_t index, uint8_t data);
int mouse_getX();
int mouse_getY();

#endif
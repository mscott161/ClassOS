#include "mouse.h"
#include "status.h"
#include "kernel.h"
#include "task/process.h"
#include "task/task.h"
#include "classic.h"
#include "io/io.h"

static struct mouse* mouse_list_head = 0;
static struct mouse* mouse_list_last = 0;

void mouse_init()
{
    mouse_insert(classic_mice_init());
}

int mouse_insert(struct mouse* mouse)
{
    int res = 0;
    if (mouse->init == 0)
    {
        res = -EINVARG;
        goto out;
    }

    if (mouse_list_last)
    {
        mouse_list_last->next = mouse;
        mouse_list_last = mouse;
    }
    else
    {
        mouse_list_head = mouse;
        mouse_list_last = mouse;
    }

    res = mouse->init();

out:
    return res;
}

void mouse_data(uint8_t index, uint8_t data)
{
  print("mouse data\n");
    struct process* process = process_current();
    process->mouse.mouse_byte[index] = data;
}

int mouse_getX()
{
    if (!task_current())
    {
        return 0;
    }

    struct process* process = task_current()->process;

    int x = process->mouse.mouse_byte[1];
    
    return x;
}

int mouse_getY()
{
    if (!task_current())
    {
        return 0;
    }

    struct process* process = task_current()->process;

    int y = process->mouse.mouse_byte[2];
    
    return y;
}


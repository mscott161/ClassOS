#include "os.h"
#include "string.h"
#include "stdio.h"

struct command_argument* os_parse_command(const char* command, int max)
{
    struct command_argument* root_command = 0;
    char scommand[1025];
    if (max >= (int)sizeof(scommand))
    {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));
    char* token = strtok(scommand, " ");
    if (!token)
    {
        goto out;
    }

    root_command = os_malloc(sizeof(struct command_argument));
    if (!root_command)
    {
        goto out;
    }

    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    struct command_argument* current = root_command;
    token = strtok(0x00, " ");
    while (token != 0x00)
    {
        struct command_argument* new_command = os_malloc(sizeof(struct command_argument));
        if (!new_command)
        {
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(0x00, " ");
    }

out:
    return root_command;
}

int os_getkeyblock()
{
    int val = 0;

    do
    {
        val = os_getkey();
    }
    while (val == 0);

    return val;
}

void os_terminal_readline(char* out, int max, bool output_while_typing)
{
    int i = 0;

    for (i = 0; i < max; i++)
    {
        char key = os_getkeyblock();

        // Carriage return means we have read the line
        if (key == 13)
        {
            break;
        }

        if (output_while_typing)
        {
            os_putchar(key);
        }

        // Backspace
        if (key == 0x08 && i >= 1)
        {
            out[i - 1] = 0x00;
            i -= 2;
            // -2 because we will +1 when we go to the continue
            continue;
        }

        out[i] = key;
    }

    // String Null terminator
    out[i] = 0x00;
}

int os_system_run(const char* command)
{
    char buf[1024];
    strncpy(buf, command, sizeof(buf));
    struct command_argument* root_command_argument = os_parse_command(buf, sizeof(buf));
    if (!root_command_argument)
    {
        return -1;
    }

    return os_system(root_command_argument);
}
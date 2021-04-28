#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0

#define KEYBOARD_SHIFT_RIGHT_DOWN 1
#define KEYBOARD_SHIFT_RIGHT_UP 0
#define KEYBOARD_SHIFT_LEFT_DOWN 1
#define KEYBOARD_SHIFT_LEFT_UP 0


typedef int KEYBOARD_CAPS_LOCK_STATE;
typedef int KEYBOARD_SHIFT_RIGHT_STATE;
typedef int KEYBOARD_SHIFT_LEFT_STATE;

struct process;

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct keyboard
{
    KEYBOARD_INIT_FUNCTION init;
    char name[20];
    
    KEYBOARD_CAPS_LOCK_STATE capslock_state;
    KEYBOARD_SHIFT_RIGHT_STATE right_shift_state;
    KEYBOARD_SHIFT_LEFT_STATE left_shift_state;

    struct keyboard* next;
};

void keyboard_init();
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();
int keyboard_insert(struct keyboard* keyboard);
void keyboard_set_capslock(struct keyboard* keyboard, KEYBOARD_CAPS_LOCK_STATE state);
KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard* keyboard);
// void keyboard_set_right_shift(struct keyboard* keyboard, KEYBOARD_SHIFT_RIGHT_STATE state);
// KEYBOARD_SHIFT_RIGHT_STATE keyboard_get_right_shift(struct keyboard* keyboard);
// void keyboard_set_left_shift(struct keyboard* keyboard, KEYBOARD_SHIFT_LEFT_STATE state);
// KEYBOARD_SHIFT_LEFT_STATE keyboard_get_left_shift(struct keyboard* keyboard);

#endif
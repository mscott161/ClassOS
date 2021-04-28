[BITS 32]

section .asm

global print:function
global os_getkey:function
global os_malloc:function
global os_free:function
global os_putchar:function
global os_process_load_start:function
global os_process_get_arguments: function
global os_system: function
global os_exit: function
global os_getmousex: function
global os_getmousey: function

; void print(const char* message)
print:
    push ebp

    mov ebp, esp
    push dword[ebp + 8]
    mov eax, 1 ; Command print
    int 0x80
    add esp, 4

    pop ebp
    ret

; int getkey()
os_getkey:
    push ebp

    mov ebp, esp
    mov eax, 2 ; Command Getkey
    int 0x80

    pop ebp
    ret

; void putchar(char c)
os_putchar:
    push ebp

    mov ebp, esp
    mov eax, 3 ; Command PutChar
    push dword[ebp + 8] ; Variable "C"
    int 0x80
    add esp, 4

    pop ebp
    ret

; void* os_malloc(size_t size)
os_malloc:
    push ebp
    
    mov ebp, esp
    mov eax, 4 ; Command Malloc (allocates memory for the process)
    push dword[ebp+8] ; Variable "size"
    int 0x80
    add esp, 4

    pop ebp
    ret

; void os_free(void* ptr)
os_free:
    push ebp

    mov ebp, esp
    mov eax, 5 ; Command 5 free (Frees the allocated memory for this process)
    push dword[ebp + 8] ; Variable "ptr"
    int 0x80
    add esp, 4

    pop ebp
    ret

; void os_process_load_start(const char* filename)
os_process_load_start:
    push ebp

    mov ebp, esp
    mov eax, 6 ; Command 6 process load start (starts a process)
    push dword[ebp + 8] ; Variable filename
    int 0x80
    add esp, 4

    pop ebp
    ret

; int os_system(struct command_argument* arguments)
os_system:
    push ebp;

    mov ebp, esp
    mov eax, 7  ; Command 7 process system (run a sysltem command based on arguments)
    push dword[ebp + 8] ; variable arguments
    int 0x80

    add esp, 4
    pop ebp
    ret

; void os_process_get_arguments(struct process_arguments* arguments)
os_process_get_arguments:
    push ebp;

    mov ebp, esp
    mov eax, 8  ; Command 8 get the process arguments
    push dword[ebp + 8] ; variable arguments
    int 0x80

    add esp, 4
    pop ebp
    ret

; void os_exit()
os_exit:
    push ebp;

    mov ebp, esp
    mov eax, 9  ; Command 9 process exit
    int 0x80

    pop ebp
    ret

; int os_getmousex()
os_getmousex:
    push ebp;

    mov ebp, esp
    mov eax, 10  ; Command 10 get Mouse Buffer
    int 0x80

    pop ebp
    ret

; int os_getmousey()
os_getmousey:
    push ebp;

    mov ebp, esp
    mov eax, 11  ; Command 10 get Mouse Buffer
    int 0x80

    pop ebp
    ret

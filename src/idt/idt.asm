section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

enable_interrupts:
  sti
  ret

disable_interrupts:
  cli
  ret

idt_load:
  push ebp  ; this is the value passed and pushes it to ebp
  mov ebp, esp
  mov ebx, [ebp+8] ; moves the value plus 8 this points to the first parameter passed
  lidt [ebx]
  pop ebp
  ret

no_interrupt:
  pushad ; pushes eax, ecx, edx, ebx
  call no_interrupt_handler
  popad ; pops eax, ecx, edx, ebx
  iret

%macro interrupt 1
  global int%1
  int%1:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
    ; uint32_t ip
    ; uint32_t cs
    ; uint32_t flags
    ; uint32_t sp
    ; uint32_t ss
    pushad ; Pushes the general purpose registers to the stack (eax, ebx, ecx, edx, etc..)
    ; INTERRUPT FRAME ENDS
    push esp
    push dword %1
    call interrupt_handler
    add esp, 8
    popad
    iret
%endmacro

%assign i 0
%rep 512
  interrupt i
%assign i i+1
%endrep


isr80h_wrapper:
  ; INTERRUPT FRAME START
  ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
  ; uint32_t ip
  ; uint32_t cs
  ; uint32_t flags
  ; uint32_t sp
  ; uint32_t ss
  
  pushad ; Pushes the general purpose registers to the stack (eax, ebx, ecx, edx, etc..)

  ; INTERRUPT FRAME ENDS

  ; Push the stack pointer so that we are pointing to the interrupt frame
  push esp

  ; EAX Holds the command, so push it to the stack for isr80h_handler
  push eax 

  call isr80h_handler
  mov dword[tmp_res], eax

  add esp, 8 ; Add extra 8 bytes to the stack pointer

  ; Restore general purpose registers for user land
  popad

  mov eax, [tmp_res] ; move the result back to eax
  iretd

section .data
; Inside here is stored the return results from isr80h_handler
tmp_res: dd 0

%macro interrupt_array_entry 1
  dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
  interrupt_array_entry i
%assign i i+1
%endrep
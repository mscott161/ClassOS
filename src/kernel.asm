[BITS 32]

global _start
global kernel_registers

extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov ebp, 0x00200000
  mov esp, ebp

  ; Setup/Enable A20 Line
  in al, 0x92
  or al, 2
  out 0x92, al

  ; Remap the master PIC
  ;mov al, 00010001b
  ;out 0x20, al ; Tell Master PIC

  ;mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
  ;out 0x21, al

  ;mov al, 00000001b
  ;out 0x21, al

  ; End Remap the master PIC

  ; Remap the slave PIC

  ;mov al, 00010001b
  ;out 0x28, al ; Tell Slave PIC

  ;mov al, 0x28 ; Interrupt 0x28 is where slace ISR should start
  ;out 0x29, al

  ;mov al, 00000001b
  ;out 0x29, al

  ; End Remap the slave PIC

  call kernel_main

  jmp $

kernel_registers:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov gs, ax
  mov fs, ax
  ret

times 512-($ - $$) db 0

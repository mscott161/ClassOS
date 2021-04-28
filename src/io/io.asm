section .asm

global insb
global insw
global outb
global outw

insb:
  push ebp
  mov ebp, esp
  
  xor eax, eax
  mov edx, [ebp + 8] ; port to read (param)
  in al, dx

  pop ebp
  ret

insw:
  push ebp
  mov ebp, esp
  
  xor eax, eax
  mov edx, [ebp + 8] ; port to read (param)
  in ax, dx

  pop ebp
  ret

outb:
  push ebp
  mov ebp, esp

  mov eax, [ebp + 12] ; value (param)
  mov edx, [ebp + 8] ; port (param)
  out dx, al

  pop ebp
  ret

outw:
  push ebp
  mov ebp, esp

  mov eax, [ebp + 12] ; value (param)
  mov edx, [ebp + 8] ; port (param)
  out dx, ax

  pop ebp
  ret


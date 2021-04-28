ORG 0x7C00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; FAT16 Header
OEMIdentifer          db 'SCOTT OS' ; Has to be 8 characters pad spaces if needed
BytesPerSectory       dw 0x200
SectorsPerCluster     db 0x80
ReservedSectors       dw 200 ; We are loading the whole kernel here in the Reserved Sectors
FATCopies             db 0x02
RootDirEntries        dw 0x40
NumSectors            dw 0x00
MediaType             db 0xF8
SectorsPerFat         dw 0x100
SectorsPerTrack       dw 0x20
NumberOfHeads         dw 0x40
HiddenSectors         dd 0x00
SectorsBig            dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber           db 0x80
WinNTBit              db 0x00
Signature             db 0x29
VolumeID              dd 0xD105
VolumeIDString        db 'SCOTT BOOT '
SystemIDString        db 'FAT16   '

start:
  jmp 0:step2

step2:
  cli ; clear interrupts this allows the below registers to be set without interrupts
  mov ax, 0x00
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7C00
  sti ; enables interrupts

.load_protected:
  cli
  lgdt[gdt_descriptor]
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax
  jmp CODE_SEG:load32

; GDT 
gdt_start:
gdt_null:
  dd 0x0
  dd 0x0

; offset 0x8
gdt_code:   ; CS SHOULD POINT TO THIS
  dw 0xFFFF ; Segment Limit first 0-15 bits
  dw 0      ; Base first 0-15 bits
  db 0      ; Base 16-23 bits
  db 0x9A   ; Access Byte
  db 11001111b  ; High 4 bit flags and the low 4 bit flags
  db 0          ; Base 24-31 bits

; offset 0x10
gdt_data:   ; DS, SS, ES, FS, GS
  dw 0xFFFF ; Segment Limit first 0-15 bits
  dw 0      ; Base first 0-15 bits
  db 0      ; Base 16-23 bits
  db 0x92   ; Access Byte
  db 11001111b  ; High 4 bit flags and the low 4 bit flags
  db 0          ; Base 24-31 bits

gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start - 1
  dd gdt_start

[BITS 32]
load32:
  mov eax, 1     ; 0 is boot sector so 1 is what we want
  mov ecx, 100   ; we want 100 sectors
  mov edi, 0x0100000  ; we want to load it at the 1 MB position
  call ata_lba_read
  jmp CODE_SEG:0x0100000

ata_lba_read:
  ; the below ASM is from documentation from OSDev about LBA 
  ; this is so we can load the kernel into memory
  ; this basic code and will create a better LBA Driver in C
  ; the out call is used because we are talking to the disk controller

  mov ebx, eax,  ; Backup the LBA
  ; Send the highest 8 bits of the lba to hard disk controller
  shr eax, 24  ; shift the eax to the right
  or eax, 0xE0 ; Select the master drive
  mov dx, 0x1F6
  out dx, al 
  ; Finished sending the highest 8 bits of the lba

  ; Send the total sectors to read
  mov eax, ecx
  mov dx, 0x1F2
  out dx, al  
  ; Finished sending the total sectors to read

  ; Send more bits of the LBA
  mov eax, ebx ; Restore the backup LBA
  mov dx, 0x1F3
  out dx, al
  ; Finsihed sending more bits of the LBA

  ; Send more bits of the LBA
  mov dx, 0x1F4
  mov eax, ebx ; Restore the backup LBA
  shr eax, 8 ; shift 8 bits to the right
  out dx, al
  ; Finished sinding more bits of the LBA

  ; Send upper 16 bits of the LBA
  mov dx, 0x1F5
  mov eax, ebx ; Restore the backup LBa
  shr eax, 16
  out dx, al
  ; Finished sending upper 16 bits of the LBA

  mov dx, 0x1F7
  mov al, 0x20
  out dx, al

  ; Read all sectors into memory
.next_sector:
  push ecx

; Checking if we need to read
.try_again:
  mov dx, 0x1F7
  in al, dx
  test al, 8
  jz .try_again

; We need to read 256 words at a time
  mov ecx, 256
  mov dx, 0x1F0
  rep insw
  pop ecx
  loop .next_sector
  ; end of reading sectors into memory
  ret

times 510-($ - $$) db 0
dw 0xAA55

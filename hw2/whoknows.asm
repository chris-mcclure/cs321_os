;
;  Tiny boot block example
;  Dr. Orion Lawlor, lawlor@alaska.edu, 2019-02-01 (Public Domain)

BITS 16 ; awful segmented memory
ORG 0x07C00 ; bios loads this boot block to linear address 0x07C00 

start:
  cli ; disable interrupts until we're set up
	; Zero out all the segment registers
	xor ax, ax ; ax=0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0xfff0 ; put stack pointer at end of our 64K area

  mov si, 4 * 0x32; take over interrupt 0x32
  mov WORD[si], my_int_code ;my code offset
  mov WORD[si+2], 0 ; code segment

	sti ; allow interrupts now that everything's set up


  call read_disk
  mov al,[0xC0DE] ; we just read disk data to this memory
  int 0x32

  call 0xC0DE

  ;call start_sector1
  
hang: jmp hang  ; wait here (forever)

my_int_code:
  pusha
  push ds
  mov ax, 0
  mov ds, ax

  ;mov al, 'h'
  mov al,[0xC0DE] ; we just read disk data to this memory

  call printchar

  pop ds
  popa
  iret

read_disk:
  mov ah,0x42
  mov si, my_DAP
  ; mov dl, 0x80 ; C drive (in a boož sector, the BIOS loads dl with the boot drive)
  int 0x13 ; disk I/O
  ret

my_DAP:
  db 0x10 ; size
  db 0 ; padding
  dw 10 ; sectors to read
  dw 0x7e00 ; memory target offset
  dw 0 ; memory target segment
  dq 1 ; read this sector

;***** Utility: Text output functions **********
; Print one character, passed in al.
;   Trashes: bx
printchar:
	; See http://stanislavs.org/helppc/int_10-e.html
	mov bx, 0x0007  ; 'output character' selector
	mov ah, 0x0e; Output a character
	int 0x10  ; "output character" INT 10,E
	ret
	
	
; pad to make this boot sector exactly 512 bytes long total
	times 512-2-($-start) db 0x90  

; A boot sector's last two bytes must contain this magic flag.
	dw 0xAA55 ; Magic value needed by BIOS
; ------- end boot sector -----


; Sector 1: 0x7c00 + 512 (=0x7e00)
start_sector1:
  
	;mov si,print_str --------COMMENTED OUT
  mov si, sectors

  str_loop:
    mov al,BYTE [si]
    cmp al,0
    je hang
    call printchar
    add si,1
    jmp str_loop

  
print_str:
	db ' This is code from SECTOR 1, loaded and operational!',0

sectors:
  db 0xbe, 0x00, 0x00, 0x8a, 0x84, 0xf3, 0xc0, 0x3c, 0x00, 0x74, 0x09, 0x34, 0xa5, 0xcd, 0x32, 0x83
  db 0xc6, 0x01, 0xeb, 0xef, 0xc3, 0xe7, 0xc0, 0x85, 0xd6, 0xd0, 0xd7, 0xc0, 0x85, 0xd1, 0xca, 0x85
  db 0xc1, 0xd7, 0xcc, 0xcb, 0xce, 0x85, 0xdc, 0xca, 0xd0, 0xd7, 0x85, 0xea, 0xd3, 0xc4, 0xc9, 0xd1
  db 0xcc, 0xcb, 0xc0, 0x89, 0x85, 0xf7, 0xc4, 0xc9, 0xd5, 0xcd, 0xcc, 0xc0, 0x84, 0x85, 0x85, 0x85
  db 0xe6, 0xf6, 0x85, 0x96, 0x97, 0x94, 0x85, 0xcd, 0xd2, 0x97, 0x8b, 0x95, 0x85, 0xc4, 0xcb, 0xd6
  db 0xd2, 0xc0, 0xd7, 0x9f, 0x85, 0x95, 0xdd, 0xc3, 0x95, 0x95, 0xc1, 0x93, 0x95, 0x95, 0xc1, 0x00
times (512 - 6*16) db 0


; VirtualBox wants a .hdd image to be padded to full 512 byte blocks
times 512-(($-start_sector1)&0x1ff) db 0x90  



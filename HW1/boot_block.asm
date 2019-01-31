;Chris McClure
;CS321
;HW1
;Goals: Read memory at a location, 
;write memory at a location, does the stack work?
;Using Dr. Lawlors utility functions below 

BITS 16 ; 16 bit mode

mov ax, 0X07C0 ;bios loads this sector to this segment
mov ds, ax 

mov ax, 0XB800 ;segment for text screen
mov es, ax

mov cx, 0
mov si, 1
teal_mode:  
	mov BYTE[es:si], 11
	add si, 2
	add cx, 1; i++
	cmp cx, 80*24 ; limit
	jl teal_mode


mov si, mystr
call printstrln

cmdloop:
	mov si, cmdprompt
	call printstr

	call readchar

;write to memory
	cmp al, 'w'
	jne not_write
		call readhex ;reads hex code provided
		push ax ;saves ax from being trashed
		call println
		pop ax ;restores ax 
		push ax
		call printhex32
		pop ax
		mov es, ax ;moving ax into es is the only way to write to memory
		mov si, 0 ; stack index we want to write to
		mov BYTE[es:si], 0x5a ;move 'Z' into the first byte of memory
		mov al, 'Z' ; print z for verification
		call printchar
		call println

		jmp cmdloop
	not_write:

; read memory address
	cmp al, 'r'
	jne not_read
		call readhex ;read hex code provided
		mov es, ax
		mov si, 0

	mov cx, 0
	start_print_loop:
		mov al, BYTE[es:si]
		push cx ;loop counter
		call printhex8
		mov al, ' ' ; space hex bytes apart
		call printchar
		pop cx
		add si, 1 ; next byte in memory
		add cx, 1 ; i++
		cmp cx, 16
		jne start_print_loop

		call println

		jmp cmdloop


	not_read:
;stack segment overrite
	cmp al,'s'
	jne not_stack
		call readhex
		mov ss, ax
		mov sp, 0x100
		call printhex32
		call println

	not_stack:
		call println
		mov si, errorinsult
		call printstrln

		jmp cmdloop


waithere: jmp waithere

mystr:
	db "Help Me", 0

cmdprompt:
	db "C:\>", 0

errorinsult:
	db "dang", 0

; ------------ Simple utility functions (call/ret interface) -------------
; Print one character, passed in al.
;   Trashes: ax, bx
printchar:
  mov bx, 0x0007  ; 'output character' selector
  mov ah, 0x0e; Output a character
  int 0x10  ; "output character" interrupt
  ret

; Print a string, starting address in si
;   Trashes: ax, bx, si
printstr:
  lodsb ; = mov al, BYTE [ds:si]
  cmp al,0 
  je print_done
  call printchar
  jmp printstr
  print_done: ret

; Print a string (from the pointer si), then a newline
printstrln:
  call printstr
  ; no ret, so falls through!
println: ; print just the newline
  mov al,13 ; CR
  call printchar
  mov al,10 ; LF
  call printchar
  ret

; Print eax as 8 hex digits onscreen
;  Trashes: ax,bx,cx
printhex32:
  mov cl,8*4 ; bit counter: print 8 hex digits, at 4 bits each
  jmp start
printhex8: ; print al as 2 hex digits onscreen
  mov cl,2*4 ; 2 hex digits
start:
    sub cl,4 ; move down 1 hex digit = 4 bits
    push eax
    
    ; print hex digit from bit number cl
    shr eax,cl ; shift ax's high bits down first
    and al,0xF ; extract the low hex digit
    cmp al,9
    jbe print_number_only
      add al,'A'-10-'0' ; print as ASCII letter
    print_number_only:
    add al,'0' ; convert number to ascii
    call printchar
    
    pop eax
    cmp cl,0
    jne start ; repeat until cl == 0
  
  ret

; Read one char from the user, return ASCII in al
;  Trashes: only ax
readchar:  
  xor ax, ax; set ax to zero (wait for keypress)
  int 0x16  ; Wait for a keypress
  call printchar ; Always echo user input
  ret

; Read hex digits from the user until hits spacebar, return value in eax
;  Trashes: ax, dx
readhex:
  xor edx, edx ; value we're accumulating
  
  read_hex_loop:
    call readchar
    cmp al,' ' ; we hit a space character
    je read_hex_done
    cmp al,13 ; we hit a (DOS) newline character
    je read_hex_done
    
    ; Else it's a real char: start by shifting old chars up
    shl edx,4 ; 4 bits per hex char
    
    cmp al,'9'
    jle read_hex_number
       ; else it's a letter
       cmp al,'F'
       jle read_hex_uppercase
        ; lowercase
        sub al,'a'-10
        jmp read_hex_add
       
       read_hex_uppercase:
        ; uppercase
        sub al,'A'-10
        jmp read_hex_add
      
      read_hex_number: ; it's a number
      sub al,'0'
    read_hex_add:
      movzx eax,al ; sign-extend al from 8 bits to 16 bits
      add edx,eax
      jmp read_hex_loop
  
read_hex_done:
  mov eax,edx ; return result in eax
  ret

; Restart the machine, by jumping to the BIOS's power-on code.
reboot:
  jmp  0xffff:0x0000 ; Jump to BIOS reset code

times 510-($-$$) db 0; magic number 
db 0x55, 0xAA 		 ;($ is where we are now, $$ is the address of the start of the file )

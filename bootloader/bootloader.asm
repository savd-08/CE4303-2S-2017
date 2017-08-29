[bits 16]
[ORG 0x7c00]		;0x07c00 is the physical address where the bootloader starts 

jmp reset			;Jump to reset tag
reset:          	;BIOS sets DL to boot drive before jumping to the bootloader

    xor ax, ax		;Data Segment is set accordingly in primary accumulator (ax)
    mov ds, ax      ;DS=0

    cli              ;Disable interrupts, to avoid problem with CPU 8088 error
                      	
    mov ss, ax       ;SS=0x0000
    mov sp, 0x7c00   ;SP=0x7c00 We'll set the stack starting just below
                     	
    sti              

    xor ax,ax   	;0=Reset device disk
    int 0x13
    jc reset        ;Carry flag was set, try again

    mov ax,0x1000   ;We are going to read address 0x1000
    mov es,ax       ;Set ES with 0x1000

device:
    xor bx,bx   	;Ensure that the buffer offset is 0
    mov ah,0x2  	;Read device=2
    mov al,0x1  	;Reading one sector
    mov ch,0x0  	;Track 1
    mov cl,0x2  	;Sector 2, track 1
    mov dh,0x0  	;Head 1
    int 0x13
    jc device   	;Carry flag was set, try again
    jmp 0x1000:0000 ;Jump to 0x1000, start of the pacman game

times 510-($-$$) db 0;Fill the rest of sector with 0
dw 0xAA55   		;This is the boot signature

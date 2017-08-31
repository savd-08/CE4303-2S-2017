[BITS 16]
[ORG 0x0000]      ; This code is intended to be loaded starting at 0x1000:0x0000
                  ; Which is physical address 0x10000. ORG represents the offset
                  ; from the beginning of our segment.

; Our bootloader jumped to 0x1000:0x0000 which sets CS=0x1000 and IP=0x0000
; We need to manually set the DS register so it can properly find our variables
; like 'var'

mov ax, cs
mov ds, ax       ; Copy CS to DS (we can't do it directly so we use AX temporarily)



 
main:
	mov ah, 0x00 	;Set video mode
	mov al, 0x13	;graphics, 320x200 res, 8x8 pixel box
	int 0x10

	mov ah, 0x0c	;Write graphics pixel
	mov bh, 0x00 	;page #0
	;int 0x10

	;Create the large boxes
	mov al, 0x8		;Set dark gray color
	mov cx, 20		;Set column (x) to 20
	mov dx, 30		;Set row (y) to 20
	call draw_large_box

	add cx, 100		;Set column (x) to 120
	call draw_large_box

	add cx, 100		;Set column (x) to 220
	call draw_large_box

	mov cx, 20		;Set column (x) to 20
	mov dx, 110		;Set row (y) to 130
	call draw_large_box

	add cx, 100		;Set column (x) to 120
	call draw_large_box

	add cx, 100		;Set column (x) to 220
	call draw_large_box


	;Fill the map with dots
	mov cx, 20		;Set column (x) to 20
	mov dx, 10		;Set row (y) to 0
	call fill_dots_x

	mov cx, 0		;Set column (x) to 0
	add dx, 80		;Set row (y) to 90
	call fill_dots_x

	mov cx, 0		;Set column (x) to 0
	add dx, 80		;Set row (y) to 180
	call fill_dots_x

	mov cx, 0		;Set column (x) to 0
	mov dx, 30		;Set row (y) to 20
	call fill_dots_y

	add cx, 100		;Set column (x) to 100
	mov dx, 30		;Set row (y) to 20
	call fill_dots_y

	add cx, 100		;Set column (x) to 200
	mov dx, 30		;Set row (y) to 20
	call fill_dots_y

	add cx, 100		;Set column (x) to 300
	mov dx, 30		;Set row (y) to 20
	call fill_dots_y

	mov word [pacman_x], 0
	mov word [pacman_y], 10
	mov word [index], 0

	mov word ax, [index]

	mov word cx, [pacman_x]	;Set column (x) to 0
	mov word dx, [pacman_y]	;Set row (y) to 0
	mov byte [pacman_color], 0xe
	pusha
	call draw_pac
	popa




	jmp loop

draw_large_box:
	pusha			;Push registers onto the stack
	int 0x10		;Draw initial pixel
	mov bx, cx		;Move initial x position to bx
	add bx, 80		;Add 80 to determine the final position of the block
	call draw_line_x	;Draw top horizontal line
	sub cx, 80		; Substract 80 to obtain initial value
	add dx, 60		; Add 70 to determine the position of the down horizontal line
	call draw_line_x 	; Draw bottom horizontal line
	sub dx, 60		; Substract 70 to obtain initial value
	sub cx, 80		; Substract 80 to obtain initial value
	mov bx, dx		; Move dx to bx
	add bx, 60		; Add 70 to obtain final value
	call draw_line_y	;Draw left vertical line
	add cx, 80		; Add 80 to obtain second vertical line initial position
	sub dx, 60		; Substract 70 to obtain initial value
	call draw_line_y	;Draw right vertical line
	popa			;Pops registers from the stack
	ret				; Return

;Horizontal line from cx to bx
draw_line_x:
	cmp cx, bx 		;Compare if currrent x equals desired x
	je return		;Returns if true
	inc cx			;Increments x coordinate (cx)
	int 0x10 		;Writes graphics pixel
	jmp draw_line_x	;Loops to itself


;Vertical line from dx to bx
draw_line_y:
	cmp dx, bx 		;Compare if currrent y equals desired y
	je return		;Returns if true
	inc dx			;Increments y coordinate (dx)
	int 0x10 		;Writes graphics pixel
	jmp draw_line_y	;Loops to itself

;Return from procedure
return:
	ret

;Draw a 4x4 dot in the center of a 20x20 grid
draw_dot:
	pusha
	mov al, 0x6		;Set brown color
	add cx, 9		;Set x as cx+9
	add dx, 9		;Set y as dx+9
	int 0x10 		;Draw the x,y pixel
	inc cx			;Set x as cx+10
	int 0x10 		;Draw the x,y pixel
	dec cx			;Set x as cx+9
	inc dx 			;Set y as dx+10
	int 0x10 		;Draw the x,y pixel
	inc cx			;Set x as cx+10
	int 0x10 		;Draw the x,y pixel
	popa
	ret

fill_dots_x:
	cmp cx, 300		;Compare if currrent x is greater than the desired x
	jg return 		;Returns if true
	call draw_dot	;Draws a dot in the selected 20x20 grid
	add cx, 20		;Adds 32 to cx to get the required dots
	jmp fill_dots_x	;Loops to itself

fill_dots_y:
	cmp dx, 170		;Compare if currrent y is greater than the desired y
	jg return 		;Returns if true
	call draw_dot	;Draws a dot in the selected 20x20 grid
	add dx, 20		;Adds 45 to dx to get the required dots
	jmp fill_dots_y	;Loops to itself

draw_pac:
	mov ah, 0x0c	;Write graphics pixel
	add cx, 4		;X starting point
	add dx, 9		;Y starting point
	mov bx, dx		;Move dx to bx
	mov al, 0		;Set al to 0 

draw_pac_loop_l:
	cmp al, 6		;Loop for 6 iterations
	je draw_pac_loop_r
	inc cx			;Increment x coordinate
	dec dx 			;Decrement y coordinate
	inc bx 			;Increment vertical line length
	pusha			;Push registers to the stack
	mov al, [pacman_color]		;Set pacman color
	call draw_line_y
	popa			;Pop registers from the stack
	inc al			;Increment al
	jmp draw_pac_loop_l		;Loop to itself

draw_pac_loop_r:
	cmp al, 0		;Loop for 6 iterations
	je return
	inc cx			;Increment x coordinate
	inc dx 			;Increment y coordinate
	dec bx 			;Decrement vertical line length
	pusha			;Push registers to the stack
	mov al, [pacman_color]		;Set pacman color
	call draw_line_y
	popa			;Pop registers from the stack
	dec al			;Decrement al
	jmp draw_pac_loop_r		;Loop to itself


get_input:
	mov ah, 0		;Set ah to 0
	int 0x16		;Get keystroke interrupt
	mov word cx, [pacman_x]
	mov word dx, [pacman_y]
	cmp ah, 0x48	;Jump if up arrow pressed
	je move_up
	cmp ah, 0x4d	;Jump if right arrow pressed
	je move_right		
	cmp ah, 0x4b	;Jump if left arrow pressed
	je move_left
	cmp ah, 0x50	;Jump if down arrow pressed
	je move_down
	jmp get_input

move_up:
	cmp dx, 10
	je get_input
	pusha
	mov byte [pacman_color], 0x0
	call draw_pac
	popa
	sub dx, 20
	mov word [pacman_y], dx
	pusha
	mov byte [pacman_color], 0xe
	call draw_pac
	popa
	ret

move_down:
	cmp dx, 170
	je get_input
	pusha
	mov byte [pacman_color], 0x0
	call draw_pac
	popa
	add dx, 20
	mov word [pacman_y], dx
	pusha
	mov byte [pacman_color], 0xe
	call draw_pac
	popa
	ret

move_left:
	cmp cx, 0
	je get_input
	pusha
	mov byte [pacman_color], 0x0
	call draw_pac
	popa
	sub cx, 20
	mov word [pacman_x], cx
	pusha
	mov byte [pacman_color], 0xe
	call draw_pac
	popa
	ret

move_right:
	cmp cx, 300
	je get_input
	mov word ax, [index]
	inc ax
	mov word bx, [pos_array + ax*4]
	cmp bx, 2
	je get_input
	pusha
	mov byte [pacman_color], 0x0
	call draw_pac
	popa
	add cx, 20
	mov word [pacman_x], cx
	pusha
	mov byte [pacman_color], 0xe
	call draw_pac
	popa
	ret



end:
	inc cx

loop:
	pusha
	call get_input
	popa
	jmp loop

section .bss
	pacman_x	resw 1
	pacman_y 	resw 1
	index 		resw 1
	pacman_color resb 1


section .data

	pos_array	db 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,2,2,2, 2,1,2,2, 2,2,1,2, 2,2,2,1
				db 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1

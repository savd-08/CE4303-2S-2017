[BITS 16]
[ORG 0x0000]      ; This code is intended to be loaded starting at 0x1000:0x0000
                  ; Which is physical address 0x10000. ORG represents the offset
                  ; from the beginning of our segment.

; Our bootloader jumped to 0x1000:0x0000 which sets CS=0x1000 and IP=0x0000
; We need to manually set the DS register so it can properly find our variables

section .text

xor ax, ax
mov ds, ax 	; Copy CS to DS (we can't do it directly so we use AX temporarily)

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

  ;draw a car
  mov word [car_x], 25        ;saves the car x coordinate
  mov word [car_y], 14        ;saves the car y coordinate
  mov al, 0x04                ;set the red color for the rectangle
  call draw_car

  ;draw a bus
  mov word [bus_w], 20        ;saves the bus width
  mov word [bus_h], 11        ;saves the bus height
  mov word [bus_x], 45        ;saves the bus x coordinate
  mov word [bus_y], 14        ;saves the bus y coordinate
  mov al, 0x0C                ;set the red color for the rectangle
  call draw_bus

  ;draw a truck
  mov word [truck_w], 30      ;saves the bus width
  mov word [truck_h], 11      ;saves the bus height
  mov word [truck_x], 85      ;saves the truck x coordinate
  mov word [truck_y], 14      ;saves the truck y coordinate
  mov al, 0x02                ;set the red color for the rectangle
  call draw_truck

	mov word [pacman_x], 0
	mov word [pacman_y], 10
	mov word [points], 0

	mov word cx, [pacman_x]	;Set column (x) to 0
	mov word dx, [pacman_y]	;Set row (y) to 0
	call draw_pac

	jmp game

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

draw_rectangle:
	int 0x10										;draws the first pixel
	;draws the top line
	pusha												;saves the registers
	mov word bx, [rectangle_w]	;gets the rectangle width
	add bx, cx									;calculate the x boundary
	call draw_line_x						;draws the line
	popa												;resotores the registers
	;draws the bottom line
	pusha												;saves the registers
	mov word bx, [rectangle_h] 	;gets the rectangle height
	add dx, bx									;obtains the initial y for bottom line
	int 0x10										;draws the first pixel
	mov word bx, [rectangle_w]	;gets the rectangle width
	add bx, cx									;calculate the x boundary
	call draw_line_x						;draws the line
	popa												;resotores the registers
	;draws the left line
	pusha												;saves the registers
	mov word bx, [rectangle_h] 	;gets the rectangle height
	add bx, dx									;obtains the y boundary
	call draw_line_y						;draws the line
	popa												;resotores the registers
	;draws the right line
	pusha												;saves the registers
	mov word bx, [rectangle_w]	;gets the rectangle width
	add cx, bx									;get the x for the right line
	mov word bx, [rectangle_h] 	;gets the rectangle height
	add bx, dx									;obtains the y boundary
	call draw_line_y						;draws the line
	popa												;resotores the registers
	ret 												;return

draw_car:
  mov word [car_w], 10        ;saves the car width
  mov word [car_h], 11        ;saves the car height
  mov word bx, [car_w]        ;load the car width
  mov word [rectangle_w], bx  ;saves the width for make a rectangle
  mov word bx,[car_h]         ;loads the car height
  mov word [rectangle_h], bx  ;saves the height for make a rectangle
  mov word cx, [car_x]        ;set the initial x
  mov word dx, [car_y]        ;set the initial y
  call draw_rectangle
  ret

draw_bus:
  mov word bx, [bus_w]        ;load the bus width
  mov word [rectangle_w], bx  ;saves the width for make a rectangle
  mov word bx,[bus_h]         ;loads the bus height
  mov word [rectangle_h], bx  ;saves the height for make a rectangle
  mov word cx, [bus_x]        ;set the initial x
  mov word dx, [bus_y]        ;set the initial y
  call draw_rectangle
  ret

draw_truck:
  mov word bx, [truck_w]      ;load the bus width
  mov word [rectangle_w], bx  ;saves the width for make a rectangle
  mov word bx,[truck_h]       ;loads the bus height
  mov word [rectangle_h], bx  ;saves the height for make a rectangle
  mov word cx, [truck_x]      ;set the initial x
  mov word dx, [truck_y]      ;set the initial y
  call draw_rectangle
  ret

draw_pac_c:
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
	jmp ret_input

move_up:
	cmp dx, 10		;Do not move if on top border
	je get_input
	pusha			;Push registers to the stack
	add cx, 10		;Center the x axis for collision detection
	call check_col	;Check if collision course
	sub dx, 10		;Look 10 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	sub dx, 20		;Update the pacman_y value in memory
	mov word [pacman_y], dx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

move_down:
	cmp dx, 170		;Do not move if on bottom border
	je get_input
	pusha			;Push registers to the stack
	add cx, 10		;Center the x axis for collision detection
	add dx, 20		;Look 20 pixels ahead for collisions
	call check_col
	add dx, 10		;Look 30 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	add dx, 20		;Update the pacman_y value in memory
	mov word [pacman_y], dx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

move_left:
	cmp cx, 0		;Do not move if on left border
	je get_input
	pusha			;Push registers to the stack
	add dx, 10		;Center the y axis for collision detection
	call check_col 	;Check if collision course
	sub cx, 10		;Look 10 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	sub cx, 20		;Update the pacman_y value in memory
	mov word [pacman_x], cx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

move_right:
	cmp cx, 300		;Do not move if on right border
	je get_input
	pusha			;Push registers to the stack
	add cx, 20		;Look 20 pixels ahead for collisions
	add dx, 10		;Center the y axis for collision detection
	call check_col
	add cx, 9		;Look 9 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	add cx, 20		;Update the pacman_y value in memory
	mov word [pacman_x], cx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop


draw_pac:
	pusha			;Push registers from the stack
	mov byte [pacman_color], 0xe	;Setting yellow color for pacman
	call draw_pac_c
	popa			;Pop registers from the stack
	ret

clear_pac:
	pusha			;Push registers from the stack
	mov byte [pacman_color], 0x0 	;Setting black color for pacman
	call draw_pac_c
	popa			;Pop registers from the stack
	ret

check_col:
	mov ah, 0x0d	;Get graphics pixel video mode
	mov bh, 0x0 	;Page 0
	int 0x10 		;BIOS Video interrupt
	cmp al, 0x8 	;If pixel is gray, collision
	je get_input
	ret

check_points:
	mov ah, 0x0d	;Get graphics pixel video mode
	mov bh, 0x0 	;Page 0
	int 0x10  		;BIOS Video interrupt
	cmp al, 0x6 	;If pixel is brown, collision
	je inc_points	;Increase points
	ret

inc_points:
	pusha			;Push registers from the stack
	mov ah, 0x0c	;Drawing a "progress bar"
	mov cx, [points]
	mov dx, 0
	call draw_dot
	popa			;Push registers from the stack
	inc word [points]	;Increment the points counter
	ret

;Game main loop
game:
	call get_input		;Check for user input
	ret_input:
	cmp word [points], 71	;Game ends when player eats all of the pellets
	je halt
	jmp game


halt:
	jmp halt


section .bss
	pacman_x	resw 1
	pacman_y 	resw 1
	pacman_color resb 1
	points 		resw 1
  rectangle_w resw 1
  rectangle_h resw 1
  car_x resw 1
  car_y resw 1
  bus_x resw 1
  bus_y resw 1
  truck_x resw 1
  truck_y resw 1
  car_w resw 1
  car_h resw 1
  bus_w resw 1
  bus_h resw 1
  truck_w resw 1
  truck_h resw 1
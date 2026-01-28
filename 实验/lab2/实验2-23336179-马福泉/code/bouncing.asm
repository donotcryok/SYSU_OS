org 0x7c00
[bits 16]

section .data
    counter db 9           
    digit_string db 48
    digit_color dw 00000010b
    cursor_x dw 2
    cursor_y dw 0
    max_x dw 80*2-2
    max_y dw 25*2-3
    x_direction_flag db 1
    y_direction_flag db 1
    short_delay dw 0x100

_start:
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov sp, 0x7c00
    mov ax, 0xb800
    mov gs, ax

proc:
    mov al, [counter]
    add al, '0'                       ;用 ASCII 
    mov byte [digit_string], al

    add word [digit_color], 00100010b ;换颜色
    
    jmp count_address

count_address:
    cmp byte [x_direction_flag], 1
    je add_value_x
    jmp sub_value_x
add_value_x:
    inc word [cursor_x]
    inc word [cursor_x]
    mov ax, [cursor_x]
    cmp ax, [max_x]
    jle y_count
    mov byte [x_direction_flag], 0
    jmp y_count
sub_value_x:
    dec word [cursor_x]
    dec word [cursor_x]
    cmp word [cursor_x], 2
    jge y_count
    mov byte [x_direction_flag], 1
    jmp y_count
y_count:
    cmp byte [y_direction_flag], 1
    je add_value_y
    jmp sub_value_y
add_value_y:
    inc word [cursor_y]
    inc word [cursor_y]
    mov ax, [cursor_y]
    cmp ax, [max_y]
    jle proc1
    mov byte [y_direction_flag], 0
    jmp proc1
sub_value_y:
    dec word [cursor_y]
    dec word [cursor_y]
    cmp word [cursor_y], 0
    jge proc1
    mov byte [y_direction_flag], 1
    jmp proc1

proc1:
    mov eax, 0
    mov ebx, 0
    mov ax, [cursor_y]
    mov bx, 80
    mul bx
    add ax, [cursor_x]
    mov bx, ax

    mov eax, 0
    mov ax, bx
    mov dh, [digit_color]
    mov dl, [digit_string]
    mov WORD [gs:eax], dx

    ; 时延
    mov cx, short_delay
delay_loop:
    nop
    mov bx, short_delay
loop2:
    dec bx
    cmp bx, 0
    nop
    jne loop2
    nop
    loop delay_loop
    
    dec byte [counter]
    cmp byte [counter], 0
    je reset_counter
    jmp proc

reset_counter:
    mov byte [counter], 9    ; Reset counter to 9
    jmp proc

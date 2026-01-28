[bits 32]

global asm_show_string

asm_show_string:
    push eax
    xor eax, eax
    mov ah, 0x12  ; 设置高字节为0x12（根据需要调整）

    mov al, '2'
    mov [gs:2 * 1290], ax
    mov al, '3'
    mov [gs:2 * 1291], ax
    mov al, '3'
    mov [gs:2 * 1292], ax
    mov al, '3'
    mov [gs:2 * 1293], ax
    mov al, '6'
    mov [gs:2 * 1294], ax
    mov al, '1'
    mov [gs:2 * 1295], ax
    mov al, '7'
    mov [gs:2 * 1296], ax
    mov al, '9'
    mov [gs:2 * 1297], ax
    mov al, 'M'
    mov [gs:2 * 1298], ax
    mov al, 'F'
    mov [gs:2 * 1299], ax
    mov al, 'Q'
    mov [gs:2 * 1300], ax
    pop eax
    ret

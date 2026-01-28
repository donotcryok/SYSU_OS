org 0x7c00
[bits 16]

; 初始化段寄存器
xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00

; 设置光标位置到第20行，第10列
mov ah, 02h
mov bh, 00h ; 页码
mov dh, 14h ; 行（20行，从0开始计数）
mov dl, 09h ; 列（10列，从0开始计数）
int 10h

; 获取光标位置
mov ah, 03h
mov bx, 00h ; 页码
int 10h

; 将光标位置存储到内存中
mov [0x5000], dh ; 行
mov [0x5001], dl ; 列

jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa
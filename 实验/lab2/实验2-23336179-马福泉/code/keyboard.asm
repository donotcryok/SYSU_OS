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

; 设置显示模式为文本模式
mov ah, 00h
mov al, 03h 
int 10h

; 主循环：等待键盘输入并回显
main_loop:
    mov ah, 00h  ; 读取键盘输入（等待按键）
    int 16h

    ; 回显字符
    mov ah, 0Eh 
    int 10h      ; 显示到屏幕，并自动移动光标

    jmp main_loop ; 继续循环

jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa  ; Bootloader 标志

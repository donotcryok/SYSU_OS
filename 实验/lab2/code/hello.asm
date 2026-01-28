org 0x7c00
[bits 16]

; 初始化段寄存器
xor ax, ax
mov ds, ax
mov es, ax

; 设置显示模式为文本模式
mov ah, 00h
mov al, 03h 
int 10h

; 逐字符输出学号 "23336179"
mov si, student_id  ; 加载字符串地址
print_loop:
    lodsb          
    or al, al     
    jz done       

    mov ah, 0x0E    ; 使用 INT 10h, AH=0Eh 显示字符
    int 10h         ; 调用 BIOS 显示字符
    jmp print_loop  ; 继续处理下一个字符

done:
    jmp $           ; 死循环

student_id db "23336179", 0  ; 学号字符串以 0 结尾

times 510 - ($ - $$) db 0
db 0x55, 0xaa  ; Bootloader 标识
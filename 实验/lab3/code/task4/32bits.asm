%include "boot.inc"

[bits 16]
org 0x7e00  ; 明确指定程序加载的位置

; 显示bootloader已运行的消息
mov ax, 0xb800
mov gs, ax
mov ah, 0x03 ;青色
mov ecx, bootloader_tag_end - bootloader_tag
xor ebx, ebx
mov esi, bootloader_tag
output_bootloader_tag:
    mov al, [esi]
    mov word[gs:bx], ax
    inc esi
    add ebx,2
    loop output_bootloader_tag

; 初始化GDT
; 空描述符
mov dword [GDT_START_ADDRESS+0x00],0x00
mov dword [GDT_START_ADDRESS+0x04],0x00  

; 数据段描述符 (0-4GB)
mov dword [GDT_START_ADDRESS+0x08],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x0c],0x00cf9200    ; 粒度为4KB，存储器段描述符 

; 堆栈段描述符      
mov dword [GDT_START_ADDRESS+0x10],0x00000000    ; 基地址为0x00000000，界限0x0 
mov dword [GDT_START_ADDRESS+0x14],0x00409600    ; 粒度为1个字节

; 显存段描述符   
mov dword [GDT_START_ADDRESS+0x18],0x80007fff    ; 基地址为0x000B8000，界限0x07FFF 
mov dword [GDT_START_ADDRESS+0x1c],0x0040920b    ; 粒度为字节

; 代码段描述符
mov dword [GDT_START_ADDRESS+0x20],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x24],0x00cf9800    ; 粒度为4kb，代码段描述符 

; 初始化描述符表寄存器GDTR
mov word [pgdt], 39      ; 描述符表的界限   
lgdt [pgdt]
      
; 打开A20地址线
in al,0x92                         ; 南桥芯片内的端口 
or al,0000_0010B
out 0x92,al                        ; 打开A20

cli                                ; 关中断
mov eax,cr0
or eax,1
mov cr0,eax                        ; 设置PE位
      
; 进入保护模式
jmp dword CODE_SELECTOR:protect_mode_begin

; 32位保护模式代码
[bits 32]           
protect_mode_begin:                              
; 设置段寄存器
mov eax, DATA_SELECTOR                     ; 加载数据段(0..4GB)选择子
mov ds, eax
mov es, eax
mov eax, STACK_SELECTOR
mov ss, eax
mov eax, VIDEO_SELECTOR
mov gs, eax

; 显示进入保护模式的消息
mov ecx, protect_mode_tag_end - protect_mode_tag
mov ebx, 80 * 2  ; 第二行开始位置
mov esi, protect_mode_tag
mov ah, 0x3      ; 青色
output_protect_mode_tag:
    mov al, [esi]
    mov word[gs:ebx], ax
    add ebx, 2
    inc esi
    loop output_protect_mode_tag

; 显示学号和姓名（交替颜色）
; 起始位置设置在屏幕中央，第10行，第30列
mov ebx, (10 * 80 + 30) * 2  ; 计算起始位置
mov esi, student_info
mov ecx, student_info_end - student_info
mov ah, 0x3F    ; 青色底白色字(第一种颜色 - 0x3为青色背景，0xF为白色文字)

output_student_info:
    mov al, [esi]
    mov word[gs:ebx], ax
    xor ah, 0xCC ; 将颜色从青色底白色字(0x3F)切换到白色底青色字(0xF3)
    add ebx, 2
    inc esi
    loop output_student_info

jmp $ ; 死循环

; 数据区
pgdt dw 0
     dd GDT_START_ADDRESS

bootloader_tag db 'Bootloader is running'
bootloader_tag_end:

protect_mode_tag db 'Now in Protected Mode'
protect_mode_tag_end:

student_info db '23336179MFQ'  ; 学号 + 姓名拼音首字母
student_info_end:

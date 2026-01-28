org 0x7c00
[bits 16]

start:
    ; 初始化段寄存器
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 初始化栈指针
    mov sp, 0x7c00

    ; 设置读取参数
    mov bx, 0x7e00           ; bootloader的加载地址
    mov ax, 1                ; 逻辑扇区号第0~15位
    mov cx, 0                ; 逻辑扇区号第16~31位

    ; 转换LBA到CHS
    call lba_to_chs
    mov cl, byte [sector]
    mov dh, byte [head]
    mov ch, byte [cylinder]

    ; 设置驱动器号和功能号
    mov dl, 0x80             ; 驱动器号（硬盘）
    mov ah, 0x02             ; 功能号为0x02(读扇区)
    mov al, 5                ; 设置读取的扇区数为5

    ; 调用BIOS读取磁盘中断
    int 0x13
    jc load_error            ; 如果进位标志CF=1，说明读取出错

    ; 读取成功，跳转到bootloader
    jmp 0x0000:0x7e00

load_error:
    ; 显示出错信息
    mov si, error_msg
    call print_string
    jmp $                    ; 无限循环

; 打印字符串的简单函数
print_string:
    lodsb                    ; 加载SI指向的字节到AL，并且SI自增
    or al, al                ; 测试是否到达字符串结束（0）
    jz done                  ; 如果是0，跳转到结束
    mov ah, 0x0E             ; BIOS中断显示字符功能号
    mov bx, 0x0007           ; 页号和颜色
    int 0x10                 ; 调用BIOS中断
    jmp print_string         ; 继续打印下一个字符
done:
    ret

; 错误信息
error_msg db 'Failed to load bootloader!', 0

; LBA到CHS的转换子程序
lba_to_chs:
    ; 逻辑扇区号向CHS的转换公式：
    ; 扇区号 S = (逻辑扇区号 L % 63 (每磁道的扇区数 SPT)) + 1
    ; 磁头号 H = (L / 63) % 18 （每柱面的磁头数）
    ; 柱面号 C = (L / 63) / 18

    ; 每磁道扇区数 (SPT) = 63
    ; 每柱面磁头数 (HPC) = 18

    ; 计算扇区号 S
    xor dx, dx               ; 清零 DX
    mov si, 63               ; SPT = 63
    div si                   ; AX / SPT，商在 AX，余数在 DX
    inc dx                   ; 扇区号 S = (L % 63) + 1
    mov byte [sector], dl    ; 保存扇区号

    ; 计算磁头号 H
    xor dx, dx               ; 清零 DX
    mov di, 18               ; HPC = 18
    div di                   ; AX / HPC，商在 AX，余数在 DX
    mov byte [head], dl      ; 磁头号 H = (L / 63) % 18

    ; 计算柱面号 C
    mov byte [cylinder], al  ; 柱面号 C = (L / 63) / 18

    ret

; 数据区
cylinder db 0
head db 0
sector db 0

times 510 - ($ - $$) db 0    ; 填充0到510字节
dw 0xAA55                    ; 启动扇区标志
section .data
    ; 数据段可以放置你需要使用的全局变量，已在test.cpp中声明

section .text
    global student_function
    global your_function

    extern a1
    extern a2
    extern my_random
    extern your_string
    extern if_flag
    extern while_flag
    extern print_a_char

student_function:
    pushad

    ; -------------------- 分支逻辑实现 --------------------
your_if:
    ; 比较 a1 < 12
    mov eax, [a1]
    cmp eax, 12
    jl if_case_1 ; 如果 a1 < 12，跳转到if_case_1

    ; 比较 a1 < 24
    cmp eax, 24
    jl if_case_2 ; 如果 12 <= a1 < 24，跳转到if_case_2

    ; 否则a1 >= 24
    ; if_flag = a1 << 4
    shl eax, 4
    mov [if_flag], eax
    jmp if_end

if_case_1:
    ; if_flag = a1 / 2 + 1
    mov eax, [a1]
    shr eax, 1          ; a1 / 2
    add eax, 1          ; +1
    mov [if_flag], eax
    jmp if_end

if_case_2:
    ; if_flag = (24 - a1) * a1
    mov eax, 24
    sub eax, [a1]       ; 24 - a1
    mov ebx, [a1]       ; 将 a1 保存到 ebx 中
    mul ebx             ; eax = (24 - a1) * a1
    mov [if_flag], eax

if_end:
    ; -------------------- 循环逻辑实现 --------------------
your_while:
    ; while a2 >= 12
    mov eax, [a2]
    cmp eax, 12
    jl while_end        ; 如果 a2 < 12，结束循环

    ; 调用 my_random
    call my_random
    
    ; 获取while_flag指针和a2的值
    mov edi, [while_flag]  ; 获取指针的值（指向的地址）
    mov ebx, [a2]
    sub ebx, 12           ; 计算索引 a2-12
    
    ; 把返回的字符(在al中)存储到while_flag[a2-12]
    mov byte [edi + ebx], al
    
    ; a2--
    dec dword [a2]
    jmp your_while

while_end:

    popad
    ret

your_function:
    ; 函数实现：遍历字符数组 string
    pushad
    mov esi, [your_string]  ; 使用指针的值，而不是指针本身

your_function_loop:
    ; 获取 string[i]
    mov al, byte [esi]
    test al, al               ; 检查是否到达字符串结尾
    jz your_function_end

    ; 调用print_a_char显示字符
    movzx eax, al            ; 扩展al到eax
    push eax
    call print_a_char
    add esp, 4               ; 清理栈

    ; 继续循环
    inc esi
    jmp your_function_loop

your_function_end:
    popad
    ret

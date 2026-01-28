#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 内存管理器
MemoryManager memoryManager;


extern "C" void setup_kernel()
{
    // 中断管理器
    interruptManager.initialize();

    // 输出管理器
    stdio.initialize();

    // 内存管理器
    memoryManager.openPageMechanism();
    
    *(int*)0x100000 = 1;
    
    
    asm_halt(); // 这行代码不会执行，因为之前的代码会触发段错误
}


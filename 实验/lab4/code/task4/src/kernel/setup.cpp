#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "Marquee.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager int_manager;
// 跑马灯对象
Marquee marquee;

extern "C" void setup_kernel()
{
    // 中断处理部件
    int_manager.initialize();
    // 屏幕IO处理部件
    stdio.initialize();
    // 初始化跑马灯
    marquee.initialize();
    // 设置时钟中断处理函数
    int_manager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    // 开启时钟中断
    int_manager.enableTimeInterrupt();
    // 开启中断
    asm_enable_interrupt();
    asm_halt();
}


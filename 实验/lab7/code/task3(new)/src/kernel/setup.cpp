#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

// 根据第1000个页目录项计算虚拟地址
void calculateVirtualAddresses() {
    // 访问页目录的基地址 (1000 << 22 | 1000 << 12)
    uint32 pdBase = 0x3E8E8000;
    
    // 第141个页目录项的虚拟地址
    uint32 pde141_addr = pdBase + (141 * 4);
    
    // 访问页表的基地址 (1000 << 22)
    uint32 ptBase = 0x3E800000;
    
    // 第891个页表中第109个页表项的虚拟地址
    uint32 pte891_109_addr = ptBase + (891 << 12) + (109 * 4);
    
    printf("Virtual address of PDE[141]: 0x%x\n", pde141_addr);
    printf("Virtual address of PTE[891][109]: 0x%x\n", pte891_109_addr);
}

void thread1(void *arg){
    char *p1=(char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("thread 1 allocate pages:%x\n",p1);
}

void thread2(void *arg){
    char *p2=(char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("thread 2 allocate pages:%x\n",p2);
}

void thread3(void *arg){
    char *p3=(char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("thread 3 allocate pages:%x\n",p3);
}
void first_thread(void *arg)
{
    // 根据第1000个页目录项计算虚拟地址
    calculateVirtualAddresses();
    
    programManager.executeThread(thread1, nullptr, "thread1", 1);
    programManager.executeThread(thread2, nullptr, "thread2", 1);
    programManager.executeThread(thread3, nullptr, "thread3", 1);
    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 内存管理器
    memoryManager.openPageMechanism();
    memoryManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}

#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

STDIO stdio;
InterruptManager interruptManager;
ProgramManager programManager;
MemoryManager memoryManager;

void memory_test_thread(void *arg) {
    // Test 1: Allocate single page memory
    int addr1 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 1);
    addr1 += 6179;  // Add page offset
    printf("Allocated 1 page at 0x%x (with offset 6179)\n", addr1);

    int addr2 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 3);
    addr2 += 6179;  // Add page offset
    printf("Allocated 3 pages at 0x%x (with offset 6179)\n", addr2);
    
    int addr3 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 5);
    addr3 += 6179;  // Add page offset
    printf("Allocated 5 pages at 0x%x (with offset 6179)\n", addr3);
    
    int addr4 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 2);
    addr4 += 6179;  // Add page offset
    printf("Allocated 2 pages at 0x%x (with offset 6179)\n", addr4);
    
    
    printf("Releasing 3 pages at address: 0x%x (original address: 0x%x)\n", addr2, addr2 - 6179);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr2 - 6179, 3);
            
    printf("Releasing 5 pages at address: 0x%x (original address: 0x%x)\n", addr3, addr3 - 6179);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr3 - 6179, 5);
    
    printf("Releasing 1 page at address: 0x%x (original address: 0x%x)\n", addr1, addr1 - 6179);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr1 - 6179, 1);
            
    printf("Releasing 2 pages at address: 0x%x (original address: 0x%x)\n", addr4, addr4 - 6179);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr4 - 6179, 2);   
      
    printf("\nMemory released successfully (Page offset: 6179)\n");
}

void first_thread(void *arg) {
    // Create memory test thread
    int pid = programManager.executeThread(memory_test_thread, nullptr, "mem_test", 1);
    if (pid != -1) {
        printf("Created memory test thread, PID: %d\n", pid);
    } else {
        printf("Failed to create memory test thread\n");
    }
    
    // Main thread cannot return
    asm_halt();
}

extern "C" void setup_kernel() {
    // 初始化基本组件
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    
    stdio.initialize();
    programManager.initialize();
    
    // 初始化内存管理器并开启分页机制
    memoryManager.openPageMechanism();
    memoryManager.initialize();
    
    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1) {
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


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

// 测试结果打印函数
void printTestResult(const char *testName, bool result) {
    printf("Test Case: %s - %s\n", testName, result ? "PASS" : "FAIL");
}

void first_thread(void *arg)
{
    // 清屏
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);
    
    printf("Start Physical Memory Page Allocation Test...\n\n");
    
    // 测试用例1：验证单个页的分配和释放是否正确
    printf("=== Test Case 1: Single Page Allocation and Release ===\n");
    int addr1 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 1);
    printf("Allocate 1 page, address: 0x%x\n", addr1);
    bool test1_part1 = (addr1 != 0);
    printTestResult("Single page allocation", test1_part1);
    
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr1, 1);
    printf("Release 1 page, address: 0x%x\n", addr1);
    
    int addr1_2 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 1);
    printf("Reallocate 1 page, address: 0x%x\n", addr1_2);
    bool test1_part2 = (addr1 == addr1_2);
    printTestResult("Single page reallocation after release", test1_part2);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr1_2, 1);
    
    printf("\n");
    
    // 测试用例2：验证多个连续页的分配和释放是否正确
    printf("=== Test Case 2: Multiple Contiguous Pages Allocation and Release ===\n");
    int addr2 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 5);
    printf("Allocate 5 pages, start address: 0x%x\n", addr2);
    bool test2_part1 = (addr2 != 0);
    printTestResult("Multi-page contiguous allocation", test2_part1);
    
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr2, 5);
    printf("Release 5 pages, start address: 0x%x\n", addr2);
    
    int addr2_2 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 5);
    printf("Reallocate 5 pages, start address: 0x%x\n", addr2_2);
    bool test2_part2 = (addr2 == addr2_2);
    printTestResult("Multi-page reallocation after release", test2_part2);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr2_2, 5);
    
    printf("\n");
    
    // 测试用例3：验证分配超过可用页时是否正确返回失败
    printf("=== Test Case 3: Allocation Beyond Available Pages ===\n");
    // 先分配一些页面，避免一次性请求太多
    int addr3 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 10);
    printf("Allocate 10 pages, start address: 0x%x\n", addr3);
    
    // 尝试分配一个超大的页数（应该会失败）
    int huge_pages = 1000000; // 一个非常大的值
    int addr3_huge = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, huge_pages);
    printf("Try to allocate %d pages, return: 0x%x\n", huge_pages, addr3_huge);
    bool test3 = (addr3_huge == 0);
    printTestResult("Excessive allocation failure detection", test3);
    
    // 释放之前分配的内存
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr3, 10);
    printf("Release 10 pages, start address: 0x%x\n", addr3);
    
    printf("\n");
    
    // 测试用例4：验证释放未分配的页是否会导致系统崩溃
    printf("=== Test Case 4: Releasing Unallocated Pages ===\n");
    // 选择一个合理但未分配的地址（这里使用之前测试中得到的地址加一个偏移）
    int unallocated_addr = addr1 + PAGE_SIZE * 100;
    printf("Try to release unallocated address: 0x%x\n", unallocated_addr);
    
    // 直接调用释放函数，然后检查系统是否仍在运行
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, unallocated_addr, 1);
    printf("Release unallocated page completed\n");
    
    // 如果代码执行到这里，说明系统没有崩溃
    bool test4 = true;
    printTestResult("Release unallocated page", test4);
    
    printf("\n=== Physical Memory Page Allocation Test Completed ===\n");

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

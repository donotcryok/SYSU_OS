#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

// 定义餐点服务相关变量
int matcha_cakes = 0;      // 抹茶蛋糕数量
int mango_cakes = 0;       // 芒果蛋糕数量
const int MAX_CAKES = 5;   // 最大蛋糕数量

Semaphore mutex;           // 互斥锁
Semaphore matcha_request;  // 抹茶蛋糕请求信号
Semaphore mango_request;   // 芒果蛋糕请求信号
Semaphore matcha_ready;    // 抹茶蛋糕准备好的信号
Semaphore mango_ready;     // 芒果蛋糕准备好的信号

// 创建延迟函数，用于故意拖延时间
void delay_function(int delay_time)
{
    while(delay_time--){
        
    }
}

// waiterA线程函数 - 提供抹茶蛋糕
void waiterA(void *arg)
{
    int id = (int)arg;
    
    while (1)
    {
        matcha_request.P();  // 等待抹茶蛋糕请求
        
        printf("Waiter-A %d: Received matcha cake request!\n", id);
        
        mutex.P();  // 获取互斥访问
        
        // 检查盘子是否已满
        if (matcha_cakes + mango_cakes < MAX_CAKES) {
            matcha_cakes++;
            printf("Waiter-A %d: Put a piece of matcha cake. (Total: %d)\n", id, matcha_cakes);
        } else {
            printf("Waiter-A %d: Plate is full, cannot add more cakes.\n", id);
        }
        
        mutex.V();  // 释放互斥访问
        matcha_ready.V();  // 通知抹茶蛋糕已准备好
        
        delay_function(0xfffffff); // 模拟准备下一个蛋糕的时间
    }
}

// waiterB线程函数 - 提供芒果蛋糕
void waiterB(void *arg)
{
    int id = (int)arg;
    
    while (1)
    {
        mango_request.P();  // 等待芒果蛋糕请求
        
        printf("Waiter-B %d: Received mango cake request!\n", id);
        
        mutex.P();  // 获取互斥访问
        
        // 检查盘子是否已满
        if (matcha_cakes + mango_cakes < MAX_CAKES) {
            mango_cakes++;
            printf("Waiter-B %d: Put a piece of mango cake. (Total: %d)\n", id, mango_cakes);
        } else {
            printf("Waiter-B %d: Plate is full, cannot add more cakes.\n", id);
        }
        
        mutex.V();  // 释放互斥访问
        mango_ready.V();  // 通知芒果蛋糕已准备好
        
        delay_function(0xfffffff); // 模拟准备下一个蛋糕的时间
    }
}

// 男性顾客线程函数 - 消费抹茶蛋糕
void male_guest(void *arg)
{
    int id = (int)arg;
    
    printf("Male guest %d: I want a matcha cake.\n", id);
    
    mutex.P();  // 获取互斥访问
    
    // 检查是否有抹茶蛋糕
    if (matcha_cakes > 0) {
        matcha_cakes--;
        printf("Male guest %d: Found a matcha cake, eating now. (Remaining: %d)\n", id, matcha_cakes);
        mutex.V();  // 释放互斥访问
    } else {
        // 没有蛋糕，发送请求
        printf("Male guest %d: No matcha cake available, requesting one...\n", id);
        mutex.V();  // 释放互斥访问
        
        matcha_request.V();  // 发送抹茶蛋糕请求
        matcha_ready.P();    // 等待抹茶蛋糕准备好
        
        mutex.P();  // 获取互斥访问
        if (matcha_cakes > 0) {
            matcha_cakes--;
            printf("Male guest %d: Got my matcha cake, eating now. (Remaining: %d)\n", id, matcha_cakes);
        } 
        mutex.V();  // 释放互斥访问
    }
    
    delay_function(0xfffffff); // 模拟食用时间
}

// 女性顾客线程函数 - 消费芒果蛋糕
void female_guest(void *arg)
{
    int id = (int)arg;
    
    printf("Female guest %d: I want a mango cake.\n", id);
    
    mutex.P();  // 获取互斥访问
    
    // 检查是否有芒果蛋糕
    if (mango_cakes > 0) {
        mango_cakes--;
        printf("Female guest %d: Found a mango cake, eating now. (Remaining: %d)\n", id, mango_cakes);
        mutex.V();  // 释放互斥访问
    } else {
        // 没有蛋糕，发送请求
        printf("Female guest %d: No mango cake available, requesting one...\n", id);
        mutex.V();  // 释放互斥访问
        
        mango_request.V();  // 发送芒果蛋糕请求
        mango_ready.P();    // 等待芒果蛋糕准备好
        
        mutex.P();  // 获取互斥访问
        if (mango_cakes > 0) {
            mango_cakes--;
            printf("Female guest %d: Got my mango cake, eating now. (Remaining: %d)\n", id, mango_cakes);
        } 
        mutex.V();  // 释放互斥访问
    }
    
    delay_function(0xfffffff); // 模拟食用时间
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);
    
    // 初始化信号量
    mutex.initialize(1);            // 互斥锁
    matcha_request.initialize(0);   // 初始化没有请求
    mango_request.initialize(0);    // 初始化没有请求
    matcha_ready.initialize(0);     // 初始化没有准备好的抹茶蛋糕
    mango_ready.initialize(0);      // 初始化没有准备好的芒果蛋糕
    
    // 初始化蛋糕数量
    matcha_cakes = 0;
    mango_cakes = 0;
    
    printf("==========================================\n");
    printf("Starting birthday party cake service simulation...\n");
    printf("==========================================\n\n");
    
    // 步骤1：创建两个服务生线程
    printf("[Step 1] Creating two waiter threads\n");
    programManager.executeThread(waiterA, (void *)6179, "waiterA", 1);
    delay_function(0xffffff);
    
    programManager.executeThread(waiterB, (void *)6179, "waiterB", 1);
    delay_function(0xffffff);
    printf("Waiters created successfully!\n\n");
    
    // 步骤2：服务员A添加一个抹茶蛋糕
    printf("[Step 2] Waiter A6179 adds a matcha cake\n");
    matcha_request.V();           // 请求服务员A添加抹茶蛋糕
    matcha_ready.P();             // 等待蛋糕准备好
    printf("Matcha cake added by Waiter A!\n\n");
    
    // 步骤3：服务员B添加一个芒果蛋糕
    printf("[Step 3] Waiter B6179 adds a mango cake\n");
    mango_request.V();            // 请求服务员B添加芒果蛋糕
    mango_ready.P();              // 等待蛋糕准备好
    printf("Mango cake added by Waiter B!\n\n");
    
    // 步骤4：男人0请求（找到抹茶蛋糕并拿走）
    printf("[Step 4] Male guest 0 requests a matcha cake\n");
    programManager.executeThread(male_guest, (void *)0, "male_guest_0", 1);
    delay_function(0xfffffff);
    printf("Male guest 0 has taken his cake!\n\n");
    
    // 步骤5：男人1请求（找不到抹茶蛋糕，通知服务员A）
    printf("[Step 5] Male guest 1 requests (will notify Waiter A)\n");
    programManager.executeThread(male_guest, (void *)1, "male_guest_1", 1);
    delay_function(0xfffffff);
    printf("Male guest 1 has requested a cake from Waiter A!\n\n");
    
    // 步骤6：服务员A添加一个抹茶蛋糕（这一步由服务员A线程自动处理，无需额外代码）
    printf("[Step 6] Waiter A is adding a matcha cake\n");
    delay_function(0xfffffff);
    printf("Waiter A has added a matcha cake!\n\n");
    
    // 步骤7：女人0请求（找到芒果蛋糕并拿走）
    printf("[Step 7] Female guest 0 requests a mango cake\n");
    programManager.executeThread(female_guest, (void *)0, "female_guest_0", 1);
    delay_function(0xfffffff);
    printf("Female guest 0 has taken her cake!\n\n");
    
    // 步骤8：女人1请求（找不到芒果蛋糕，通知服务员B）
    printf("[Step 8] Female guest 1 requests (will notify Waiter B)\n");
    programManager.executeThread(female_guest, (void *)1, "female_guest_1", 1);
    delay_function(0xfffffff);
    printf("Female guest 1 has requested a cake from Waiter B!\n\n");
    
    // 步骤9：服务员B添加一个芒果蛋糕（这一步由服务员B线程自动处理，无需额外代码）
    printf("[Step 9] Waiter B is adding a mango cake\n");
    delay_function(0xfffffff);
    printf("Waiter B has added a mango cake!\n\n");
    
    // 步骤10：男人2请求（应该能拿到服务员A新放的抹茶蛋糕）
    printf("[Step 10] Male guest 2 requests (should get the new matcha cake)\n");
    programManager.executeThread(male_guest, (void *)2, "male_guest_2", 1);
    delay_function(0xfffffff);
    printf("Male guest 2 has taken his cake!\n\n");
    
    // 步骤11：女人2请求（应该能拿到服务员B新放的芒果蛋糕）
    printf("[Step 11] Female guest 2 requests (should get the new mango cake)\n");
    programManager.executeThread(female_guest, (void *)2, "female_guest_2", 1);
    delay_function(0xfffffff);
    printf("Female guest 2 has taken her cake!\n\n");
    
    // 步骤12：服务员A连续添加5个抹茶蛋糕
    printf("[Step 12] Waiter A adds 5 matcha cakes consecutively\n");
    for (int i = 0; i < 5; i++) {
        printf("Waiter A is adding matcha cake #%d\n", i+1);
        matcha_request.V();           // 请求服务员A添加抹茶蛋糕
        matcha_ready.P();             // 等待蛋糕准备好
        delay_function(0xffffff);
    }
    printf("Waiter A has added 5 matcha cakes!\n\n");
    
    // 步骤13：女人3请求（找不到芒果蛋糕，通知服务员B）
    printf("[Step 13] Female guest 3 requests (will notify Waiter B)\n");
    programManager.executeThread(female_guest, (void *)3, "female_guest_3", 1);
    delay_function(0xfffffff);
    printf("Female guest 3 has requested a cake from Waiter B!\n\n");
    
    // 步骤14：服务员B尝试添加一个芒果蛋糕，但是盘子已满，失败
    printf("[Step 14] Waiter B tries to add a mango cake but the plate is full\n");
    // 这一步由服务员B线程自动处理，无需额外代码
    delay_function(0xfffffff); 
    printf("Waiter B failed to add a mango cake because the plate is full!\n\n");
    
    // 模拟结束
    printf("==========================================\n");
    printf("Cake service simulation completed!\n");
    printf("==========================================\n");
    
    // 主线程继续监控
    while(1) {
        delay_function(0xffffffff);
        mutex.P();
        printf("Current status - Matcha: %d, Mango: %d, Total: %d/%d\n", 
               matcha_cakes, mango_cakes, matcha_cakes + mango_cakes, MAX_CAKES);
        mutex.V();
    }
    
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

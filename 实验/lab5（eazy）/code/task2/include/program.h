#ifndef PROGRAM_H
#define PROGRAM_H

#include "list.h"
#include "thread.h"

#define ListItem2PCB(ADDRESS, LIST_ITEM) ((PCB *)((int)(ADDRESS) - (int)&((PCB *)0)->LIST_ITEM))

class ProgramManager
{
public:
    List allPrograms;   // 所有状态的线程/进程的队列
    List readyPrograms; // 处于ready(就绪态)的线程/进程的队列
    PCB *running;       // 当前执行的线程
public:
    ProgramManager();
    void initialize();
    // 创建一个线程并放入就绪队列
    
    // 成功，返回pid；失败，返回-1
    int executeThread(ThreadFunction function, void *parameter, const char *name, int priority);

    // 分配一个PCB
    PCB *allocatePCB();
    // 归还一个PCB
    // program：待释放的PCB
    void releasePCB(PCB *program);

    // 执行线程调度
    void schedule();
    
    PCB *find_thread_ByPid(int pid);
};

void program_exit();

#endif

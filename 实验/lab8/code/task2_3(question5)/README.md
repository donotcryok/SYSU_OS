在`build`文件夹下执行下面的命令。

# 编译
```shell
make
```

# 运行
```shell
make run
```

# debug
```shell
make debug
```
class ProgramManager
{
    // ...existing code...
    
    // 根据pid查找对应的PCB
    PCB *findProgramByPid(int pid);
};
# 清除中间文件
```shell
make clean
```
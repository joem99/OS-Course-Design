# OS-Course-Design

## 实验环境
操作系统: Ubuntu 64位 18.10

Linux内核版本: 4.17.8

CMake最低版本:  3.10

## 编译
编译环境已经在CMakeLists.txt中配置，直接通过cmake编译，编译命令如下
```OS-Course-Design目录下
mkdir build
cd build
cmake ..
make
```
编译生成实验一的两个可执行文件 copy 和 procUI
实验二的两个测试程序 testhello 和 testcopy
以及实验四的可执行文件 myw_sysmonitor

## EXP1-LINUX_UI
1. 运行格式: `./copy <source_file> <target_file>`
2. 运行格式: `./procUI `（gtk2+）

## EXP2-SYSCALL
增添系统指令过程中修改的文件包括：
系统调用表：arch_x86_entry_syscalls_syscall_64.tbl 
系统调用服务例程：kernel/sys.c 

添加了两个系统调用myhello以及mycopy，
* myhello：简单的输出
* mycopy：文件拷贝

## EXP3-Device_Driver
设备驱动程序及Makefile均在该文件夹下的myw_dev目录下。

测试程序有待改进

## EXP4-System_monitor
运行：`./myw_sysmonitor`

**功能大体完善，有几个页面有待改进**

## EXP5-File_System
在build目录下运行 `./mywfs`

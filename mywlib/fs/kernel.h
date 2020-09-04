/* kernel.h contain all the function prototypes */
#ifndef _KERNEL_H
#define _KERNEL_H
/* 系统相关函数 */
void startsys();
void existsys();

/* 虚拟磁盘相关函数 */
void format();

/* 目录处理函数 */
void mkdir(char *dirname);
void cd(char *dirname);
void deldir(char *dirname);
void ls();

/* 文件处理函数 */
int create(char *filename);
void delfile(char *filename);
int open(char *filename);
void close(int fid);
int write(int fid);
int dowrite(int fid, char *text, int len, char wstyle);
int read(int fid);
#endif

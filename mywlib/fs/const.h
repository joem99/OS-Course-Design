#ifndef _CONST_H
#define _CONST_H

#define BLOCKSIZE	1024 /* 磁盘块大小1024B */
#define VERSION 	0.01 /* 文件系统版本 */
#define SIZE		10240000 /* 模拟文件系统大小10M */
#define END			65535 /* end标志 */
#define FREE		0 /* 磁盘块未使用标志 */
#define ROOTBLOCKNUM	2 /* 根目录所在磁盘块号  */
#define MAXOPENFILE	10 /* 同时能够打开的最大的文件目录数 */

#endif //_CONST_H

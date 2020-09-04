/*
 * OS Course Design - Exp 1
 * 
 * C Programing under Linux - File copy
 * 
 * Created by myw on 2020.02.17
 * 
 */

#include "../mywlib/myw_sys.h"

#define BUF_LEN 1024

mode_t get_mode(char const *file);
void do_copy(int from_fd, int to_fd);

/*
 * get_mode - 获取源文件类型
 */
mode_t get_mode(char const *file) {
	struct stat st_buf;
	if (stat(file, &st_buf) != 0)
		err_exit("Stat failed");
	return st_buf.st_mode;
}

/*
 * do_copy - 拷贝源文件到目标文件
 */
void do_copy(int from_fd, int to_fd) {
	int read_num;
	char tmp_buf[BUF_LEN];
	while(1) {
		read_num = read(from_fd, tmp_buf, BUF_LEN);
		if (read_num == -1)
			err_exit("Copy failed");
		else if (read_num == 0)
			break;
		write(to_fd, tmp_buf, read_num);
	}
}

int main(int argc, char const *argv[]) {
	int from_fd, to_fd;
	// 验证参数个数
	if (argc != 3) 
		usage_err("Usage: ./copy <source_file> <target_file>");
	// 以只读方式打开源文件
	if ((from_fd = open(argv[1], O_RDONLY)) == -1)
		err_exit("Open source file failed");
	// 以只写方式打开目标文件，若不存在则创建，存在则删除原文件
	if ((to_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, get_mode(argv[1]))) == -1)
		err_exit("Open target file failed");
	// 进行拷贝
	do_copy(from_fd, to_fd);
	// 关闭文件流
	close(from_fd);
	close(to_fd);
	return 0;
}

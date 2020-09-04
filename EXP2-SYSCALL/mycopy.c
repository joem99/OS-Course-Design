SYSCALL_DEFINE2(mycopy, const char *, source_file, const char *, target_file) {
	char tmp_buf[1024];
	char s_filename[256], t_filename[256];
	int from_fd, to_fd;
	long read_num;
	struct kstat k_buf;
	mm_segment_t oldfs;


	/* 内核中进行系统调用（如文件操作）时，必须调用下面两句，
	对其进行保护，其作用是让内核能访问用户空间 */
	oldfs = get_fs();	// 备份当前进程地址空间
	set_fs(get_ds());	// 设置进程地址空间为虚拟地址空间上限

	/* 从用户空间获取文件名 */
	/*
	 * strncpy_from_user — Copy a NUL terminated string from userspace.
	 */
	read_num = strncpy_from_user(s_filename, source_file, sizeof(s_filename));
	printk("%ld\n", read_num);
	if (read_num < 0 || read_num == sizeof(s_filename)) {
		set_fs(oldfs);	// 恢复进程地址空间
		return -EFAULT;
	}
	read_num = strncpy_from_user(t_filename, target_file, sizeof(t_filename));
	printk("%ld\n", read_num);
	if (read_num < 0 || read_num == sizeof(t_filename)) {
		set_fs(oldfs);	
		return -EFAULT;
	}

	/* 获取source_file文件类型 */
	if (vfs_stat(s_filename, &k_buf) != 0) {
		set_fs(oldfs);
		return -EFAULT;
	}

	/* 打开文件 */
	if ((from_fd = ksys_open(s_filename, O_RDONLY, 0)) == -1) {
		set_fs(oldfs);
		return -EFAULT;
	}

	printk("Source file open");
	if ((to_fd = ksys_open(t_filename, O_WRONLY | O_CREAT | O_TRUNC, k_buf.mode)) == -1) {
		set_fs(oldfs);
		return -EFAULT;
	}

	/* 进行copy */
	while(1) {
		read_num = ksys_read(from_fd, tmp_buf, sizeof(tmp_buf));
		if (read_num < 0) {
			set_fs(oldfs);
			return -EFAULT;
		} else if (read_num == 0)
			break;
		ksys_write(to_fd, tmp_buf, read_num);
	}

	/* 关闭文件 */
	ksys_close(from_fd);
	ksys_close(to_fd);

	/* 恢复进程地址空间 */
	set_fs(oldfs);

	return 0;
}
#include "sysmonitor.h"

/**********************************************************/
/***************** Processes标签页用到的函数 *****************/
/*********************************************************/

/*
 * get_processes_info - 获取所有进程的信息from /proc/pid/stat
 */
void get_processes_info(void) {
	DIR *dir; // 指向/proc/pid/stat的文件指针
	struct dirent *dir_info;
	char file_name[1024];
	char stat_file[1024];
	char *tmp_file = NULL;
	int fd;
	char proc_info[6][1024]; // 存放1个进程的6项信息 
	gchar *txt[6];

	/* 设置clist的6项表头 */
	gtk_clist_set_column_title(GTK_CLIST(clist), 0, "PID");
    gtk_clist_set_column_title(GTK_CLIST(clist), 1, "Name");
    gtk_clist_set_column_title(GTK_CLIST(clist), 2, "Status");
    gtk_clist_set_column_title(GTK_CLIST(clist), 3, "PPID");
    gtk_clist_set_column_title(GTK_CLIST(clist), 4, "Priority");
    gtk_clist_set_column_title(GTK_CLIST(clist), 5, "MEM");

    /* 设置6项表头的宽度 */
    gtk_clist_set_column_width(GTK_CLIST(clist), 0, 75);
    gtk_clist_set_column_width(GTK_CLIST(clist), 1, 125);
    gtk_clist_set_column_width(GTK_CLIST(clist), 2, 75);
    gtk_clist_set_column_width(GTK_CLIST(clist), 3, 75);
    gtk_clist_set_column_width(GTK_CLIST(clist), 4, 75);
    gtk_clist_set_column_width(GTK_CLIST(clist), 5, 75);

    /* 设置为居中对齐 */
    gtk_clist_set_column_justification(GTK_CLIST(clist), 0, GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification(GTK_CLIST(clist), 1, GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification(GTK_CLIST(clist), 2, GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification(GTK_CLIST(clist), 3, GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification(GTK_CLIST(clist), 4, GTK_JUSTIFY_CENTER);
    gtk_clist_set_column_justification(GTK_CLIST(clist), 5, GTK_JUSTIFY_CENTER);
    gtk_clist_column_titles_show(GTK_CLIST(clist));

    /* 从/proc/pid/stat 中读数据 */
    dir = opendir("/proc");
    proc_num = 0; // 记录进程的个数
    while (dir_info = readdir(dir)) {
    	// 读数字组成的文件夹
    	if ((dir_info->d_name)[0] >= '0' && ((dir_info->d_name)[0]) <= '9') {
    		sprintf(file_name, "/proc/%s/stat", dir_info->d_name); // file_name存放stat路径
    		fd = open(file_name, O_RDONLY);
    		read(fd, stat_file, 1024); // stat_file存放stat文件内容
    		close(fd);
    		tmp_file = stat_file;
    		/* 调用 read_stat 获取1个进程的6项信息 */
    		read_stat(tmp_file, proc_info);
    		for (int i = 0; i < 6; i++)
        		txt[i] = utf8_fix(proc_info[i]);
    		gtk_clist_append(GTK_CLIST(clist), txt);
    		proc_num ++;
    	}
    }
    closedir(dir);
}

/*
 * read_stat - 从/proc/pid/stat中获取需要的进程信息
 */
void read_stat(char *stat_file, char (*proc_info)[1024]) {
	/*
	 * stat文件的结构：官方manual http://man7.org/linux/man-pages/man5/proc.5.html
	 */
	int pos;
	int i;          // 获取priority和rss时用来计数
	char mem[1024]; // 暂存进程占用的内存
	/* 获取pid */
  	for (pos = 0; pos < 1024; pos++) {
  		// 以空格为标志找信息
        if (stat_file[pos] == ' ')
     	    break;
    }
    stat_file[pos] = '\0';
    strcpy(proc_info[0], stat_file);
    stat_file += (pos + 2); // 移动stat_file的文件指针

	/* 获取(comm) */
    for (pos = 0; pos < 1024; pos++) {
  		// 以右括号为标志找信息
        if (stat_file[pos] == ')')
     	    break;
    }
    stat_file[pos] = '\0';
    strcpy(proc_info[1], stat_file);
    stat_file += (pos + 2);

	/* 获取status */
    for (pos = 0; pos < 1024; pos++) {
        if (stat_file[pos] == ' ')
     	    break;
    }
    stat_file[pos] = '\0';
    strcpy(proc_info[2], stat_file);
    stat_file += (pos + 1);

	/* 获取ppid */
    for (pos = 0; pos < 1024; pos++) {
        if (stat_file[pos] == ' ')
     	    break;
    }
    stat_file[pos] = '\0';
    strcpy(proc_info[3], stat_file);
    stat_file += (pos + 1);

	/* 获取priority */
	for (i = 0, pos = 0; pos < 1024; pos++) {
        if (stat_file[pos] == ' ')
        	i ++;
        if (i == 13)
     	    break;
    }
    stat_file[pos] = '\0';
    stat_file += pos;
    stat_file += 1;
    for (pos = 0; pos < 1024; pos++) {
        if (stat_file[pos] == ' ')
        break;
    }
    stat_file[pos] = '\0';
    strcpy(proc_info[4], stat_file);
    stat_file += pos;
    stat_file += 1;

    /* 获取rss，单位：pages */
	for (i = 0, pos = 0; pos < 1024; pos++) {
        if (stat_file[pos] == ' ')
        	i ++;
        if (i == 5)
     	    break;
    }
    stat_file[pos] = '\0';
    stat_file += pos;
    stat_file += 1;
    for (pos = 0; pos < 1024; pos++) {
        if (stat_file[pos] == ' ')
        break;
    }
    stat_file[pos] = '\0';

    // 1 pages = 4KB
    if ((4 * atoi(stat_file)) > 1024)
    	sprintf(mem, "%.1f MB\0", (4 * atoi(stat_file)) / 1024.0);
    else
    	sprintf(mem, "%d KB\0", 4 * atoi(stat_file));
    strcpy(proc_info[5], mem);
}


/***************** Processes标签页的CALLBACK函数 *****************/

/*
 * select_to_entry - 用来再双击某行后将其pid填充到entry中
 */
void select_to_entry(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data) {
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &select_pid); // 获取选中行的pid
    gtk_entry_set_text(GTK_ENTRY(entry), (gchar *)select_pid); // 将pid填充到entry中
    return ;
}

/*
 * search_proc - 根据pid查找高亮相应的进程
 */
void search_proc(GtkButton *button, gpointer data) {
	const gchar *entry_text; // 用户输入的pid
	gchar *text;
	gint ret;
	gint row = 0; // 记录当前行
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry)); // 获取用户输入的pid
	/* 遍历所有进程找pid相同的 */
	while ((ret = gtk_clist_get_text(GTK_CLIST(clist), row, 0, &text)) != 0) {
		if (!strcmp(entry_text, text))
			break;
		row ++;
	}
	// 高亮对应行
	gtk_clist_select_row(GTK_CLIST(clist), row, 0);
	// 滚动scrolled_window以便显示高亮行
	scroll_line(data, proc_num, row);
	return ;
}

/*
 * kill_proc - 杀死选中进程
 */
void kill_proc(void) {
	int ret;
	if (select_pid != NULL) {
		ret = kill(atoi(select_pid), SIGKILL); // 杀死选中的进程
		// 杀死进程的权限不够,弹出提示框
		if (ret == -EPERM) {
			pop_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_window_set_title(GTK_WINDOW(pop_window), "Error!");
			gtk_widget_set_size_request(pop_window, 350, 200);
			pop_label = gtk_label_new("Need root privilege \n      to kill the process.");
			// set_label_fontsize(pop_label, "20");
			gtk_container_add(GTK_CONTAINER(pop_window), pop_label);
			gtk_widget_show_all(pop_window);
		}
	}
	return ;
}

/*
 * refresh_proc - 杀死选中进程
 */
void refresh_proc(void) {
	gtk_clist_freeze(GTK_CLIST(clist));
	gtk_clist_clear(GTK_CLIST(clist));
	get_processes_info();
	gtk_clist_thaw(GTK_CLIST(clist));
	gtk_clist_select_row(GTK_CLIST(clist), 0, 0);
	return ;
}



/**********************************************************/
/******************** CPU标签页用到的函数 ********************/
/*********************************************************/

/*
 * get_cpu_info - get cpu info from /proc/cpuinfo
 *
 * 文件格式 参考： https://blog.csdn.net/cuker919/article/details/7635488
 */
void get_cpu_info(char *cpu_name, char *cpu_cache, char *cpu_cores, char *cpu_fpu) {
	int fd;
	char cpu_info[4096];
	char tmp_info[1024];	// 暂存cpu每项信息
	int i = 0;
	char *pos = NULL;		// 指向cpuinfo

	fd = open("/proc/cpuinfo", O_RDONLY);
	read(fd, cpu_info, sizeof(cpu_info));
	close(fd);

	/* 获取model name */
	pos = strstr(cpu_info, "model name"); // 找到model name行
	while ((*pos) != ':')
		pos ++;
	pos += 2; // name 的第一个字符
	while ((*pos) != '\n') {
		tmp_info[i] = *pos;
		i ++;
		pos ++;
	}
	tmp_info[i] = '\0';
	strcpy(cpu_name, tmp_info);

	/* 获取cache size */
	pos = strstr(cpu_info, "cache size");
	while ((*pos) != ':')
		pos ++;
	pos += 2; 	
	i = 0;
	while ((*pos) != '\n') {
		tmp_info[i] = *pos;
		i ++;
		pos ++;
	}
	tmp_info[i] = '\0';
	strcpy(cpu_cache, tmp_info);

	/* 获取cpu cores */
	pos = strstr(cpu_info, "cpu cores");
	while ((*pos) != ':')
		pos ++;
	pos += 2; 	
	i = 0;
	while ((*pos) != '\n') {
		tmp_info[i] = *pos;
		i ++;
		pos ++;
	}
	tmp_info[i] = '\0';
	strcpy(cpu_cores, tmp_info);

	/* 获取fpu */
	pos = strstr(cpu_info, "fpu");
	while ((*pos) != ':')
		pos ++;
	pos += 2; 	
	i = 0;
	while ((*pos) != '\n') {
		tmp_info[i] = *pos;
		i ++;
		pos ++;
	}
	tmp_info[i] = '\0';
	strcpy(cpu_fpu, tmp_info);
}


/****************** CPU标签页的CALLBACK函数 ******************/

/*
 * cpu_curve_callback - 每1s刷新一次cpu曲线
 */
void cpu_curve_callback(GtkWidget *cpu_curve, GdkEventExpose *event, gpointer data) {
	static int flag = 0;
	draw_cpu_curve(cpu_curve); // 画当前cpu曲线
	if (flag == 0) {
		g_timeout_add(1000, draw_cpu_curve, cpu_curve);
		flag = 1;
	}
	return ;
}

/******************** CPU标签页的LOOPS函数 ********************/

/*
 * get_cpu_usage - calculate cpu usage from /proc/stat
 * 
 * calculate method from: http://www.samirchen.com/linux-cpu-performance/
 */
gboolean get_cpu_usage(gpointer label) { 
	static int flag = 0;
	static long old_idle, old_total; // 储存之前的idle和total值
	
	long total;						 // 当前total
	CPU_OCCUPY cpu_stat;			 // 储存/proc/stat文件中cpu信息的结构体
	float idle_diff, total_diff;     // 储存两次的差值
	char stat_buf[256], cpu_usage_buf[256];
	int fd;
	fd = open("/proc/stat", O_RDONLY);
	read(fd, stat_buf, sizeof(stat_buf));
	close(fd);
	sscanf(stat_buf, "%s %ld %ld %ld %ld %ld %ld %ld", cpu_stat.name, &cpu_stat.user, &cpu_stat.nice, &cpu_stat.system, &cpu_stat.idle, &cpu_stat.iowait, &cpu_stat.irq, &cpu_stat.softirq);

	/* 第一次计算 */
	if (flag == 0) {
		old_idle = cpu_stat.idle;
		old_total = cpu_stat.user + cpu_stat.nice + cpu_stat.system + cpu_stat.idle + cpu_stat.iowait + cpu_stat.irq + cpu_stat.softirq;
		cpu_usage = 0;
		flag = 1;
	} else { 	/* 第二次计算 */
		total = cpu_stat.user + cpu_stat.nice + cpu_stat.system + cpu_stat.idle + cpu_stat.iowait + cpu_stat.irq + cpu_stat.softirq;
		
		/* 计算cpu利用率 */
		total_diff = total - old_total;
		idle_diff = cpu_stat.idle - old_idle;
		cpu_usage = 100 * (total_diff - idle_diff) / total_diff;

		// 更新
		old_total = total;
		old_idle = cpu_stat.idle;
	}
	// 设置label
	sprintf(cpu_usage_buf, "CPU usage: %0.1f%%", cpu_usage);
	gtk_label_set_text(GTK_LABEL(label), cpu_usage_buf);
	
	return TRUE;
}

/*
 * get_cpu_mhz - get cpu mhz(当前) from /proc/cpuinfo
 */
gboolean get_cpu_mhz(gpointer label) {
	int fd;
	char cpu_info[4096];
	char tmp_info[1024];	// 暂存cpu MHz
	char cpu_MHz_buf[256];
	int i = 0;
	char *pos = NULL;		// 指向cpuinfo

	fd = open("/proc/cpuinfo", O_RDONLY);
	read(fd, cpu_info, sizeof(cpu_info));
	close(fd);

	/* 获取cpu MHz */
	pos = strstr(cpu_info, "cpu MHz");
	while((*pos) != ':')
		pos ++;
	pos += 2;
	while ((*pos) != '\n') {
		tmp_info[i] = *pos;
		i ++;
		pos ++;
	}
	tmp_info[i] = '\0';
	strcpy(cpu_MHz, tmp_info);
	
	sprintf(cpu_MHz_buf, "CPU actual Frequency: %s MHz", cpu_MHz);
	gtk_label_set_text(GTK_LABEL(label), cpu_MHz_buf);

	return TRUE;
}

/*
 * draw_cpu_curve - 画当前CPU曲线
 */
gboolean draw_cpu_curve(gpointer widget) {
	GtkWidget *cpu_curve = (GtkWidget *)widget;

	GdkColor color;
	GdkGC *gc = cpu_curve->style->fg_gc[GTK_WIDGET_STATE(cpu_curve)];

	static int flag = 0;
	static int now_pos = 0;
	int draw_pos = 0;

	/* 画曲线图的背景(white) */
	color.red = 0xffff;
  	color.green = 0xffff;
  	color.blue = 0xffff;
  	gdk_gc_set_rgb_fg_color(gc, &color);
  	gdk_draw_rectangle(cpu_curve->window, gc, TRUE, 15, 30, 480, 200);

  	/* 背景线(shadow blue) */
  	color.red = 0xe000;
    color.green = 0xffff;
    color.blue = 0xffff;
    gdk_gc_set_rgb_fg_color(gc, &color);
    for (int i = 30; i <= 220; i += 20)
        gdk_draw_line(cpu_curve->window, gc, 15, i, 495, i);
    for (int i = 15; i <= 480; i += 20)
        gdk_draw_line(cpu_curve->window, gc, i + cpu_curve_start, 30, i + cpu_curve_start, 230);

    /* 设置曲线起始点 */
    cpu_curve_start -= 4;
    if (cpu_curve_start == 0)
    	cpu_curve_start = 20;

    /* 初始化数据 */
    if (flag == 0) {
    	for (int i = 0; i < 120; i++) {
    		cpu_usage_data[i] = 0;
    		flag = 1;
    	}
    }

    /* 添加cpu usage数据 */
    cpu_usage_data[now_pos] = cpu_usage / 100.0;
    now_pos ++;
    if (now_pos == 120)
    	now_pos = 0;

    /* 画曲线 */
    color.red = 0;
	color.green = 0xbf00;
	color.blue = 0xffff;
	gdk_gc_set_rgb_fg_color(gc, &color);
    draw_pos = now_pos;
    for (int i = 0; i < 119; i++) {
    	gdk_draw_line(cpu_curve->window, gc,
                  15 + i * 4, 230 - 200 * cpu_usage_data[draw_pos % 120],
                  15 + (i + 1) * 4, 230 - 200 * cpu_usage_data[(draw_pos + 1) % 120]);
   		draw_pos++;
    	if (draw_pos == 120)
        	draw_pos = 0;
    }

    /* 重置color（不然所有颜色都是之前的颜色） */
    color.red = 25000;
	color.green = 25000;
	color.blue = 25000;
	gdk_gc_set_rgb_fg_color(gc, &color);

	return TRUE;
}


/**********************************************************/
/******************** MEM标签页用到的函数 ********************/
/*********************************************************/

/****************** MEM标签页的CALLBACK函数 ******************/

/*
 * mem_curve_callback - 每1s刷新一次mem曲线
 */
void mem_curve_callback(GtkWidget *mem_curve, GdkEventExpose *event, gpointer data) {
	static int flag = 0;
	draw_mem_curve(mem_curve); 
	if (flag == 0) {
		g_timeout_add(1000, draw_mem_curve, mem_curve);
		flag = 1;
	}
	return ;
}

/******************** MEM标签页的LOOPS函数 ********************/


/*
 * get_mem_used - 显示已使用mem及总量
 */
gboolean get_mem_used(gpointer label) {

	char mem_used[1024];

	/* 直接用get_mem_info() 获得的信息 */
	sprintf(mem_used, "Memory : %0.2f / %0.2f GB", mem_total - mem_free, mem_total);
	gtk_label_set_text(GTK_LABEL(label), mem_used);

	return TRUE;
}

/*
 * get_mem_usage - 计算mem使用率
 */
gboolean get_mem_usage(gpointer label) {
	char mem_usage_buf[1024];

	/* 计算MEM利用率 */
	mem_usage = 100 - (mem_free / mem_total) * 100; // 这种计算方式更准确
	sprintf(mem_usage_buf, "MEM usage : %0.1f%%", mem_usage);
	gtk_label_set_text(GTK_LABEL(label), mem_usage_buf);

	return TRUE;
}

/*
 * get_mem_info - get mem info from /proc/meminfo
 * 
 * 文件格式：https://langzi989.github.io/2016/12/19/通过-proc-meminfo实时获取系统内存使用情况/
 * 
 * 实现方法与 get_cpu_info()基本一致
 */
gboolean get_mem_info(gpointer label) { 
	/* Used memory = MemTotal - MemFree */

	int fd;
	char mem_info[1024];
	char mem_total_buf[1024];	
	char mem_free_buf[1024];	
	char mem_available_buf[1024];	
	char swap_total_buf[1024];	
	char swap_free_buf[1024];	
	char mem_buf[1024]; 			// 最后显示的信息

	int i = 0;
	char *pos = NULL;		

	fd = open("/proc/meminfo", O_RDONLY);
	read(fd, mem_info, sizeof(mem_info));
	close(fd);

	/* 获取MemTotal */
	pos = strstr(mem_info, "MemTotal"); 
	while ((*pos) != ':')
		pos ++;
	pos += 1; 
	while ((*pos) == ' ') {
		pos ++;
	}
	while ((*pos) != ' ') {
		mem_total_buf[i] = *pos;
		i ++;
		pos ++;
	}
	mem_total_buf[i] = '\0';
	mem_total = atof(mem_total_buf) / (1024 * 1024); // 转换为GB保存到mem_total

	/* 获取MemFree（没用到的物理内存） */
	pos = strstr(mem_info, "MemFree");
	while ((*pos) != ':')
		pos ++;
	pos += 1;
	while ((*pos) == ' ')
		pos ++;

	i = 0;
	while ((*pos) != ' ') {
		mem_free_buf[i] = *pos;
		i ++;
		pos ++;
	}
	mem_free_buf[i] = '\0';
	mem_free = atof(mem_free_buf) / (1024 * 1024);

	/* 获取MemAvailable */
	pos = strstr(mem_info, "MemAvailable");
	while ((*pos) != ':')
		pos ++;
	pos += 1;
	while ((*pos) == ' ')
		pos ++;

	i = 0;
	while ((*pos) != ' ') {
		mem_available_buf[i] = *pos;
		i ++;
		pos ++;
	}
	mem_available_buf[i] = '\0';
	mem_available = atof(mem_available_buf) / (1024 * 1024);

	/* 获取SwapTotal */
	pos = strstr(mem_info, "SwapTotal");
	while ((*pos) != ':')
		pos ++;
	pos += 1;
	while ((*pos) == ' ')
		pos ++;

	i = 0;
	while ((*pos) != ' ') {
		swap_total_buf[i] = *pos;
		i ++;
		pos ++;
	}
	swap_total_buf[i] = '\0';
	swap_total = atof(swap_total_buf) / (1024 * 1024);

	/* 获取SwapFree */
	pos = strstr(mem_info, "SwapFree");
	while ((*pos) != ':')
		pos ++;
	pos += 1;
	while ((*pos) == ' ')
		pos ++;

	i = 0;
	while ((*pos) != ' ') {
		swap_free_buf[i] = *pos;
		i ++;
		pos ++;
	}
	swap_free_buf[i] = '\0';
	swap_free = atof(swap_free_buf) / (1024 * 1024);


    sprintf(mem_buf, "Total Memory : %0.2f GB\n\nAvailable Memory : %0.2f GB\n\nTotal Swap : %0.2f GB\n\nFree Swap : %0.2f GB\n", 
    					mem_total, mem_available, swap_total, swap_free);
    gtk_label_set_text(GTK_LABEL(label), mem_buf);

	return TRUE;
}


/*
 * draw_mem_curve - 画当前MEM曲线
 */
gboolean draw_mem_curve(gpointer widget) {

	GtkWidget *mem_curve = (GtkWidget *)widget;

	GdkColor color;
	GdkGC *gc = mem_curve->style->fg_gc[GTK_WIDGET_STATE(mem_curve)];

	static int flag = 0;
	static int now_pos = 0;
	int draw_pos = 0;

	/* 画曲线图的背景(white) */
	color.red = 0xffff;
  	color.green = 0xffff;
  	color.blue = 0xffff;
  	gdk_gc_set_rgb_fg_color(gc, &color);
  	gdk_draw_rectangle(mem_curve->window, gc, TRUE, 15, 30, 480, 200);

  	/* 背景线(shadow blue) */
  	color.red = 0xe000;
    color.green = 0xffff;
    color.blue = 0xffff;
    gdk_gc_set_rgb_fg_color(gc, &color);
    for (int i = 30; i <= 220; i += 20)
        gdk_draw_line(mem_curve->window, gc, 15, i, 495, i);
    for (int i = 15; i <= 480; i += 20)
        gdk_draw_line(mem_curve->window, gc, i + mem_curve_start, 30, i + mem_curve_start, 230);

    /* 设置曲线起始点 */
    mem_curve_start -= 4;
    if (mem_curve_start == 0)
    	mem_curve_start = 20;

    /* 初始化数据 */
    if (flag == 0) {
    	for (int i = 0; i < 120; i++) {
    		mem_usage_data[i] = 0;
    		flag = 1;
    	}
    }

    /* 添加mem usage数据 */
    mem_usage_data[now_pos] = mem_usage / 100.0;
    now_pos ++;
    if (now_pos == 120)
    	now_pos = 0;

    /* 画曲线 */
    color.red = 0;
	color.green = 0xbf00;
	color.blue = 0xffff;
	gdk_gc_set_rgb_fg_color(gc, &color);
    draw_pos = now_pos;
    for (int i = 0; i < 119; i++) {
    	gdk_draw_line(mem_curve->window, gc,
                  15 + i * 4, 230 - 200 * mem_usage_data[draw_pos % 120],
                  15 + (i + 1) * 4, 230 - 200 * mem_usage_data[(draw_pos + 1) % 120]);
   		draw_pos++;
    	if (draw_pos == 120)
        	draw_pos = 0;
    }

    /* 重置color（不然所有颜色都是之前的颜色） */
    color.red = 25000;
	color.green = 25000;
	color.blue = 25000;
	gdk_gc_set_rgb_fg_color(gc, &color);

	return TRUE;
}


/**********************************************************/
/******************** SYS标签页用到的函数 ********************/
/*********************************************************/

/******************** SYS标签页的LOOPS函数 ********************/

/*
 * get_local_info - get local info from /etc/hostname /etc/issue
 *
 * /proc/sys/kernel/osrelease /proc/version ...
 */
gboolean get_local_info(gpointer label) {
	int fd;
	int i, j;
	FILE *fp;
	char tmp_buf[1024];

	char host_name[128];
	char os_name[128];
	int os_type;
	char os[128];
	char kernel_version[128];
	int setup_time;
	int setup_hour, setup_min, setup_sec;

	/* 获取主机名 */
	fp  = fopen("/etc/hostname", "r");
	fgets(host_name, sizeof(host_name), fp);
	fclose(fp);

	/* 获取操作系统类型 */
	memset(tmp_buf, 0, sizeof(tmp_buf));
	fd = open("/etc/issue", O_RDONLY);
	read(fd, tmp_buf, sizeof(tmp_buf));
	close(fd);
	for (i = 0; i < 1024; i++) {
		if (tmp_buf[i] == '\\')
			break;
	}
	tmp_buf[i - 1] = '\0';
	strcpy(os_name, tmp_buf);

	/* 获取操作系统位数 */
	os_type = sizeof(char *) * 8;
	sprintf(os, "%s %d", os_name, os_type);

	/* 获取内核版本 */
	fp = fopen("/proc/sys/kernel/osrelease", "r");
	fgets(kernel_version, sizeof(kernel_version), fp);
	fclose(fp);

	/* 获取启动时间 */
	memset(tmp_buf, 0, sizeof(tmp_buf));
	fd = open("/proc/uptime", O_RDONLY);
	read(fd, tmp_buf, sizeof(tmp_buf));
	close(fd);
	for (i = 0; i < 1024; i++) {
		if (tmp_buf[i] == ' ')
			break;
	}
	tmp_buf[i] = '\0';
	setup_time = atoi(tmp_buf);
	setup_sec = setup_time % 60;
	setup_min = (setup_time / 60) % 60;
	setup_hour = setup_time / 3600;

    sprintf(tmp_buf, "Hostname:          %s\n\
OS:           %s-bit\n\n\
Kernel Version:    %s\n\
Uptime:              %02d:%02d:%02d",
          host_name, os, kernel_version, setup_hour, setup_min, setup_sec);

    gtk_label_set_text(GTK_LABEL(label), tmp_buf);

    return TRUE;
}



int main(int argc, char const *argv[]) {
	gtk_init(&argc, (char***)&argv);

	/* 所需控件 */
	GtkWidget *main_window;       // 程序窗口
	GtkWidget *notebook;    	  // notebook控件
	GtkWidget *vbox;			  // 纵向盒用来储存每个页面的多种信息
	GtkWidget *hbox;			  // 横向盒用来
	GtkWidget *hbox2;			  // 横向盒用来
	GtkWidget *frame;			  // 盒中子构件
	GtkWidget *page_label;		  // 用于显示标签页的名
	GtkWidget *label1;			  // 用于在frame中显示信息
	GtkWidget *label2;
	GtkWidget *label3;
	GtkWidget *scrolled_window;	  // 显示所有进程的滚动窗口
	GtkWidget *button1;			  // 管理进程的3个button
	GtkWidget *button2;
	GtkWidget *button3;
	GtkWidget *cpu_load;
	GtkWidget *mem_load;
	GtkWidget *image;

	/* 储存信息的buf */
	char page_title[1024];  	  // 储存每个页面的标签名
	char cpu_buf[1024];	  	      // cpu info
	// char mem_buf[1024];			  // mem info
	char author_buf[1024];		  // 显示author信息
	char version_buf[1024];		  // 显示author信息

	/* 储存从cpuinfo获取的信息 */
	char cpu_name[1024];	 	  // cpu model name
	char cpu_cache[1024];         // cpu cache size
	char cpu_cores[1024];         // cpu cores
	char cpu_fpu[1024];			  // cpu fpu

	/* 创建窗口 */
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "MYW System Monitor");
	g_signal_connect(G_OBJECT (main_window), "delete_event",G_CALLBACK (gtk_main_quit), NULL);
	gtk_widget_set_size_request(main_window, WIDTH, HEIGHT);
	gtk_container_set_border_width(GTK_CONTAINER(main_window), BORDER_WIDTH); //设置窗口边框宽度

	/* 创建notebook控件 */
	notebook = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(main_window), notebook); // 将notebook控件放入窗口中
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP); // 标签页放在顶部



	/*
	 *	Page 1 - Processes info
	 */
	sprintf(page_title, "Processes");
	page_label = gtk_label_new(page_title);

	vbox = gtk_vbox_new(FALSE, 10);

	/* 创建一个滚动窗口；有需要时才出现滚动条 */
	scrolled_window = gtk_scrolled_window_new(NULL,NULL);
	// gtk_widget_set_size_request(scrolled_window, 550, 500);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	// gtk_widget_set_size_request(scrolled_window, 500, 500);

	/* scrolled_window一共显示进程的六项信息 */
	clist = gtk_clist_new(6);
	get_processes_info(); // 获得所有进程的信息
	// 捕捉 “select_row”信号
	gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(select_to_entry), NULL);
	// clist控件放入scrolled_window中
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), clist);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0); // scrolled_window放入vbox中

	/* vbox中scrolled_window下，用hbox储存，用来搜索、杀死、刷新进程 */
	hbox = gtk_hbox_new(FALSE, 10);
	entry = gtk_entry_new(); // entry用来输入查找的进程id
	gtk_entry_set_max_length(GTK_ENTRY(entry), 0);
	button1 = gtk_button_new_with_label("Search");  // button1用于搜索进程
	button2 = gtk_button_new_with_label("Kill");    // button2用于杀死进程
	button3 = gtk_button_new_with_label("Refresh"); // button3用于刷新
    g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(search_proc), scrolled_window);
    g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(kill_proc), NULL);
    g_signal_connect(G_OBJECT(button3), "clicked", G_CALLBACK(refresh_proc), NULL);
    gtk_widget_set_size_request(entry, 200, 30);
    gtk_widget_set_size_request(button1, 80, 30);
    gtk_widget_set_size_request(button2, 80, 30);
    gtk_widget_set_size_request(button3, 80, 30);
    // 3个button控件置于hbox中
    gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), button1, FALSE, FALSE, 5);
  	gtk_box_pack_start(GTK_BOX(hbox), button2, TRUE, FALSE, 5);
  	gtk_box_pack_start(GTK_BOX(hbox), button3, FALSE, FALSE, 5);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5); // hbox置于vbox

  	/* vbox作为一个标签页附加到notebook中 */
  	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, page_label);
	

	/*
	 *	Page 2 - CPU info
	 */
  	sprintf(page_title, "CPU");
  	page_label = gtk_label_new(page_title);

  	vbox = gtk_vbox_new(FALSE, 10);

  	/* frame构件cpu_load用来存放CPU负载曲线 */
  	hbox = gtk_hbox_new(FALSE, 0);
  	cpu_load = gtk_frame_new("CPU Load"); 
  	gtk_container_set_border_width(GTK_CONTAINER(cpu_load), 5);
  	gtk_widget_set_size_request(cpu_load, 525, 300);
  	gtk_box_pack_start(GTK_BOX(hbox), cpu_load, TRUE, FALSE, 5);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

  	/* label显示CPU相关信息 */
  	hbox = gtk_hbox_new(FALSE, 0);
  	gtk_widget_set_size_request(hbox, 550, 30);
  	label1 = gtk_label_new(NULL);
  	label2 = gtk_label_new(NULL);
  	g_timeout_add(1000, get_cpu_usage, label1); // 刷新时间都为1s
  	g_timeout_add(1000, get_cpu_mhz, label2);
    gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), label2, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

    /* 画CPU利用率曲线 */
    cpu_curve = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(cpu_curve), "expose_event", G_CALLBACK(cpu_curve_callback), NULL);
    gtk_container_add(GTK_CONTAINER(cpu_load), cpu_curve); // 曲线放入cpu_load frame中


    /* frame构件用来显示CPU相关信息 */
    frame = gtk_frame_new("CPU Info");
    gtk_widget_set_size_request(frame, 500, 200);
    get_cpu_info(cpu_name, cpu_cache, cpu_cores, cpu_fpu); // get cpu info from /proc/cpuinfo 
    sprintf(cpu_buf, "CPU Type and Frequency :\n%s\n\nCache size : %s\n\nFpu : %s\n\nCPU Cores : %s\n", 
    					cpu_name, cpu_cache, cpu_fpu, cpu_cores);
    label3 = gtk_label_new(cpu_buf);
    // set_label_fontsize(label3, "14");
    gtk_container_add(GTK_CONTAINER(frame), label3);

    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, page_label);





	/*
	 *	Page 3 - Mem info
	 */
  	sprintf(page_title, "Memory");
  	page_label = gtk_label_new(page_title);

  	vbox = gtk_vbox_new(FALSE, 10);

  	/* frame构件mem_load用来存放MEM负载曲线 */
  	hbox = gtk_hbox_new(FALSE, 0);
  	mem_load = gtk_frame_new("MEM Load"); 
  	gtk_container_set_border_width(GTK_CONTAINER(mem_load), 5);
  	gtk_widget_set_size_request(mem_load, 525, 300);
  	gtk_box_pack_start(GTK_BOX(hbox), mem_load, TRUE, FALSE, 5);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

  	/* label显示MEM相关信息 */
  	hbox = gtk_hbox_new(FALSE, 0);
  	gtk_widget_set_size_request(hbox, 550, 30);
  	label1 = gtk_label_new(NULL);
  	label2 = gtk_label_new(NULL);
  	g_timeout_add(1000, get_mem_used, label1); // 刷新时间都为1s
  	g_timeout_add(1000, get_mem_usage, label2);
    gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), label2, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

    /* 画MEM利用率曲线放入mem_load中 */
    mem_curve = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(mem_curve), "expose_event", G_CALLBACK(mem_curve_callback), NULL);
    gtk_container_add(GTK_CONTAINER(mem_load), mem_curve); 

    /* frame构件用来显示MEM相关信息 */
    frame = gtk_frame_new("MEM Info");
    gtk_widget_set_size_request(frame, 500, 200);

    /* Mem info有动态数据 */
    label3 = gtk_label_new(NULL);
    g_timeout_add(1000, get_mem_info, label3);

    // set_label_fontsize(label3, "14");
    gtk_container_add(GTK_CONTAINER(frame), label3);

    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, page_label);


	/*
	 *	Page 4 - System info
	 */
	sprintf(page_title, "System");
  	page_label = gtk_label_new(page_title);

	vbox = gtk_vbox_new(FALSE, 10); // 盒中子构件大小不同

	/* frame 1 - local info */
	frame = gtk_frame_new("Local info"); // 第一个frame显示本机信息
	label1 = gtk_label_new(NULL);
	g_timeout_add(1000, get_local_info, label1); // 刷新时间1s，关联函数：get_local_info() 

	gtk_container_add(GTK_CONTAINER(frame), label1);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5); //将frame构件放入vbox中
	
	// /* frame2 - network info*/
	// frame = gtk_frame_new("Network info"); // 第二个frame显示网络信息
	// label1 = gtk_label_new(NULL);
	// gtk_container_add(GTK_CONTAINER(frame), label1);
	// g_timeout_add(1000, get_network_info, label1); 

	// gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5); 

	/* frame3 - */
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, page_label);


	/*
	 *	Page 5 - About
	 */
    sprintf(page_title, "About");
  	page_label = gtk_label_new(page_title);

  	vbox = gtk_vbox_new(FALSE, 10); 

  	/* Author 框 */
    hbox = gtk_hbox_new(FALSE, 0);
    frame = gtk_frame_new("Author");
    sprintf(author_buf, "\n\n\n\n%-15s %-25s\n\n%-15s %-25s\n\n%-15s %-25s\n\n%-15s %-25s\n\n\n\n", 
    					"Auther:", "MYW", "ID:", "U201714622", "Class:", "CS1704", "E-mail:", "mywjyw@gmail.com");
    label1 = gtk_label_new(author_buf);
    set_label_fontsize(label1, "14");
    image = gtk_image_new_from_file(AUTHOR_IMG);

    gtk_box_pack_start(GTK_BOX(hbox), image, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);  	

    /* Version 框 */
    frame = gtk_frame_new("Version");
    sprintf(version_buf, "\n\nVersion v1.1.5\n\n\
A simple system monitor\n\
Displays Processes CPU Memory and System info\n\n\n");
    label2 = gtk_label_new(version_buf);
    set_label_fontsize(label3, "14");
    gtk_container_add(GTK_CONTAINER(frame), label2);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);


    label3 = gtk_label_new("OS Course Design EXPIV");
    gtk_box_pack_start(GTK_BOX(vbox), label3, TRUE, FALSE, 5);


    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, page_label);

  	/* 显示main_window */
  	gtk_widget_show_all(main_window);
  	gtk_main();
	return 0;
}



char* utf8_fix(char *c) {
  return g_locale_to_utf8(c, -1, NULL, NULL, NULL);
}

/*
 * scroll_line - 设置scrolled_window的位置以便可以显示高亮行
 * 
 * https://my.oschina.net/plumsoft/blog/79950
 */
void scroll_line(gpointer scrolled_window, gint line_num, gint to_line_index) {
    GtkAdjustment *adj;
    gdouble lower_value, upper_value, page_size, max_value, line_height, to_value;
    adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    lower_value = gtk_adjustment_get_lower(adj);
    upper_value = gtk_adjustment_get_upper(adj);
    page_size = gtk_adjustment_get_page_size(adj);
    max_value = upper_value - page_size;
    line_height = upper_value / line_num;
    to_value = line_height * to_line_index;
    if (to_value < lower_value)
        to_value = lower_value;
    if (to_value > max_value)
        to_value = max_value;
    gtk_adjustment_set_value(adj, to_value);
    return;
}

/*
 * set_label_fontsize - To set font size in a label
 * 
 * Referencing from: https://blog.csdn.net/gl_ding/article/details/4939355
 */
void set_label_fontsize(GtkWidget *label, char *fontsize) {
  PangoFontDescription *desc_info = pango_font_description_from_string(fontsize);
  gtk_widget_modify_font(label, desc_info);
  pango_font_description_free(desc_info);
}
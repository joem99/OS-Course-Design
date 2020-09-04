/*
 * OS Course Design - Exp 1
 * 
 * C Programing under Linux - Display 3 Processes with GTK 
 * 
 * Created by myw on 2020.02.17
 * 
 */

#include "../mywlib/myw_sys.h"

#include <wait.h>
#include <gtk/gtk.h>

/* 刷新周期1000ms */
#define REFRESH_CYCLE 1000
/* 窗口大小参数 */ 
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300


/* 定义一个cpu occupy的结构体 */
typedef struct CPU_PACKED {       
	char name[20];             //定义一个char类型的数组名name有20个元素
	unsigned int user;         //定义一个无符号的int类型的user
	unsigned int nice;         //定义一个无符号的int类型的nice
	unsigned int system;       //定义一个无符号的int类型的system
	unsigned int idle;         //定义一个无符号的int类型的idle
	unsigned int iowait;
	unsigned int irq;
	unsigned int softirq;
} CPU_OCCUPY;

/* 计算CPU利用率的函数 */
// double cal_cpuOccupy (CPU_OCCUPY *o, CPU_OCCUPY *n);
// void get_cpuOccupy (CPU_OCCUPY *cpust);

/*
 * refresh_sum - 子进程1窗口内容，显示累加和
 */
gboolean refresh_sum(gpointer label) {
	static int sum = 0;
	static int now = 1;

	gchar *text_old = g_strdup_printf("<span font_desc='32'>%d+%d=</span>", sum, now);
  	
  	// 进行累加
  	sum += now++;
  	gchar *text_new = g_strdup_printf("<span font_desc='48'>%d</span>", sum);
  	
  	// 累加完成后重新开始
  	if (now == 100) {
  		sum = 0;
  		now = 1;
  	}
  	gchar *text_markup = g_strdup_printf("\n%s\n\n%s\n", text_old, text_new);

  	gtk_label_set_markup(GTK_LABEL(label), text_markup);
  	return TRUE;
}

/*
 * refresh_CPUusage - 子进程2窗口内容，显示CPU利用率
 */
gboolean refresh_CPUusage(gpointer label) { 
	static int flag = 0;
	static long old_idle, old_total; // 储存之前的idle和total值
	static float cpu_usage;

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
 	gchar *text_rate = g_strdup_printf("<span font_desc='32'>%0.1f%%</span>", cpu_usage);
  	gchar *text_markup = g_strdup_printf("\nCPU usage: %s\n", text_rate);
  	
  	gtk_label_set_markup(GTK_LABEL(label), text_markup);
	
	return TRUE;
}

/*
 * refresh_time - 父进程窗口内容，显示当前时间
 */
gboolean refresh_time(gpointer label) {
	time_t times;
	struct tm *p_time;
	time(&times);
	p_time = localtime(&times);

	gchar *text_data = g_strdup_printf("<span size='x-large'>%04d-%02d-%02d</span>",\
        (1900+p_time->tm_year),(1+p_time->tm_mon),(p_time->tm_mday));
    gchar *text_time = g_strdup_printf("<span size='xx-large'>%02d:%02d:%02d</span>",\
    (p_time->tm_hour), (p_time->tm_min), (p_time->tm_sec));
	gchar *text_markup = g_strdup_printf("\n%s\n\n%s\n", text_data, text_time);

    gtk_label_set_markup(GTK_LABEL(label), text_markup);
    return TRUE;
}



int main(int argc, char **argv) {
	int pid1, pid2; // 两个子进程号
	int wait_status;

	switch(pid1 = fork()) {
		/* fork() failed */
		case -1:
			err_exit("Fork child process 1 failed");

		/* 子进程1 - 做1到100的累加求和 */
		case 0:
			gtk_init(&argc, &argv);
			// 创建一个新窗口
			GtkWidget *window_child1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
			// 设置窗口标题
			gtk_window_set_title(GTK_WINDOW(window_child1), "Window-Child1: Show Add 1 to 100");
			// 接收关闭信号的信号槽
			g_signal_connect (G_OBJECT (window_child1), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
		
			/* 窗口内部构件 */
			GtkWidget *lable_child1 = gtk_label_new(NULL);
			// 把label放入window中
			gtk_container_add(GTK_CONTAINER(window_child1), lable_child1);
			// 规定窗口在固定时间间隔执行的函数(刷新周期3s)
			g_timeout_add(REFRESH_CYCLE * 3, refresh_sum, (void *)lable_child1);

			/* 设置窗口大小并展示 */
			gtk_widget_set_size_request(window_child1, WINDOW_WIDTH, WINDOW_HEIGHT);
			gtk_widget_show_all(window_child1);
			// 程序运行停在这等待其他事件发生
			gtk_main();
			printf("Window-Child1 closed");
			exit(0);

		default:
			switch(pid2 = fork()) {
				case -1:
					err_exit("Fork child process 2 failed");

				/* 子进程2 - 实时监测CPU的利用率 */
				case 0:
					gtk_init(&argc, &argv);

					GtkWidget *window_child2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
					gtk_window_set_title(GTK_WINDOW(window_child2), "Window-Child2: Show CPU usage");
					g_signal_connect (G_OBJECT (window_child2), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
		
					GtkWidget *lable_child2 = gtk_label_new(NULL);
					gtk_container_add(GTK_CONTAINER(window_child2), lable_child2);
					
					// 刷新周期2s
					g_timeout_add(REFRESH_CYCLE * 2, refresh_CPUusage, (void *)lable_child2);

					gtk_widget_set_size_request(window_child2, WINDOW_WIDTH, WINDOW_HEIGHT);
					gtk_widget_show_all(window_child2);

					gtk_main();
					printf("Window-Child2 closed");
					exit(0);					
				/* 父进程 - 显示当前时间 */
				default:
					gtk_init(&argc, &argv);

					GtkWidget *window_parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
					gtk_window_set_title(GTK_WINDOW(window_parent), "Window-Parent: Show Time");
					g_signal_connect (G_OBJECT (window_parent), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
		
					GtkWidget *lable_parent = gtk_label_new(NULL);
					gtk_container_add(GTK_CONTAINER(window_parent), lable_parent);
					
					// 刷新周期1s
					g_timeout_add(REFRESH_CYCLE, refresh_time, (void *)lable_parent);

					gtk_widget_set_size_request(window_parent, WINDOW_WIDTH, WINDOW_HEIGHT);
					gtk_widget_show_all(window_parent);

					gtk_main();

					/* 等待子进程结束 */
					printf("Window-Parent closed");
					waitpid(pid1, &wait_status, 0);
					waitpid(pid2, &wait_status, 0);
					exit(0);		
			}	
	}
}





// double cal_cpuOccupy (CPU_OCCUPY *o, CPU_OCCUPY *n) {
//     double od, nd;
//     double id, sd;
//     double cpu_use ;
 
//     od = (double) (o->user + o->nice + o->system +o->idle+o->softirq+o->iowait+o->irq);//第一次(用户+优先级+系统+空闲)的时间再赋给od
//     nd = (double) (n->user + n->nice + n->system +n->idle+n->softirq+n->iowait+n->irq);//第二次(用户+优先级+系统+空闲)的时间再赋给od
 
//     id = (double) (n->idle);    //用户第一次和第二次的时间之差再赋给id
//     sd = (double) (o->idle) ;    //系统第一次和第二次的时间之差再赋给sd
//     if((nd-od) != 0)
//     cpu_use =100.0- ((id-sd))/(nd-od)*100.00; //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
//     else cpu_use = 0;
//     return cpu_use;
// }
 
// void get_cpuOccupy (CPU_OCCUPY *cpust) {
//     FILE *fd;
//     int n;
//     char buff[256];
//     CPU_OCCUPY *cpu_occupy;
//     cpu_occupy = cpust;
 
//     fd = fopen ("/proc/stat", "r");
//     fgets (buff, sizeof(buff), fd);
 
//     sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle ,&cpu_occupy->iowait,&cpu_occupy->irq,&cpu_occupy->softirq);
 
//     fclose(fd);
// }
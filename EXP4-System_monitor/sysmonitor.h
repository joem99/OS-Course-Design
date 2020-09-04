#include "../mywlib/myw_sys.h"
#include <dirent.h>
#include <gtk/gtk.h>

#define WIDTH 600 
#define HEIGHT 650
#define BORDER_WIDTH 10

/* Set author face picture */
#define AUTHOR_IMG "../EXP4-System_monitor/data/author.jpg"


/* 定义一个cpu occupy的结构体 */
typedef struct CPU_PACKED {   
	char name[20];    
	long user;         
	long nice;         
	long system;       
	long idle;         
	long iowait;
	long irq;
	long softirq;
} CPU_OCCUPY;

/* 全局变量 */
GtkWidget *pop_window;
GtkWidget *pop_label;
char *select_pid = NULL;       
gint proc_num = 0;  
GtkWidget *clist;       
GtkWidget *clist2;      
GtkWidget *entry;
float cpu_usage = 0;     
float cpu_usage_data[120];  
char cpu_MHz[1024];     
float mem_total = 0;        
float mem_free = 0;         
float mem_usage = 0;        
float mem_available = 0;
float mem_usage_data[120];  
float swap_total = 0;  
float swap_free = 0;  
float swap_ratio = 0; 
float swap_ratio_data[120]; 
GtkWidget *cpu_curve;
GtkWidget *mem_curve;   
GtkWidget *swap_curve;
int cpu_curve_start = 20;   
int mem_curve_start = 20;   
int swap_curve_start = 20;  
float receive_speed = 0;    
float send_speed = 0;     
float read_speed = 0;  
float write_speed = 0;  

/* Callback functions */
void select_to_entry(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data);
void search_proc(GtkButton *button,gpointer data);
void kill_proc(void);
void refresh_proc(void);
void cpu_curve_callback(GtkWidget *cpu_curve, GdkEventExpose *event, gpointer data);
void mem_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean swap_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
void refresh_modules(void);

/* Loop functions */
gboolean draw_cpu_curve(gpointer widget);
gboolean draw_mem_curve(gpointer widget);
gboolean draw_swap_curve(gpointer widget);
gboolean get_cpu_ratio(gpointer label);
gboolean get_cpu_mhz(gpointer label);
gboolean get_memory_ratio(gpointer label);
gboolean get_memory_fraction(gpointer label);
gboolean get_swap_ratio(gpointer label);
gboolean get_swap_fraction(gpointer label);
gboolean get_sys_info(gpointer label);
gboolean get_network_info(gpointer label);
gboolean get_disk_info(gpointer label);

/* Assist functions */
char *utf8_fix(char *c);
void scroll_line(gpointer scrolled_window, gint line_num, gint to_line_index);
void set_label_fontsize(GtkWidget *label, char *fontsize);
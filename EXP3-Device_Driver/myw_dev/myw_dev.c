/*
 * OS Course Design - Exp 3
 * 
 * Device driver - Add Character device driver
 * 
 * Created by myw on 2020.02.21
 * 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_MAJOR 111 /* 主设备号 */
#define MEM_SIZE 0x4000  /* 8KB for myw driver*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("myw");
MODULE_DESCRIPTION("A Simple Character Device driver module");

static struct cdev cdev; /* 字符设备 */
static dev_t  devno;	 /* 设备号 */

/* 生成设备文件所需struct */
struct class *class;
struct device *device;

/* 字符设备中的数据 */
static char mem[MEM_SIZE] = "hello from kernel!";

/*
 * mywdriver_open - 打开设备
 */
static int mywdriver_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "MYW Driver: open\n");
    return 0;
}

/*
 * mywdriver_release - 释放设备
 */
static int mywdriver_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "MYW Driver: release\n");
    return 0;
}

/*
 * mywdriver_read - 从设备中读
 */
static ssize_t mywdriver_read(struct file *filep, char __user *buf, size_t count, loff_t *offset) {
    size_t avail;
    int ret;

    printk(KERN_INFO "MYW Driver: start read\n");

    avail = sizeof(mem) - *offset;

    /* 设备中剩余空间足够 */
    if (count <= avail) {
        if (copy_to_user(buf, mem + *offset, count) != 0) {
            return -EFAULT;
        }

        *offset += count;
        ret = count;
    } else { /* 设备中剩余空间不足 */
        if (copy_to_user(buf, mem + *offset, avail) != 0) {
            return -EFAULT;
        }

        *offset += avail;
        ret = avail;
    }

    printk(KERN_INFO "MYW Driver: read %d B", ret);
    return ret;
}

/*
 * mywdriver_write - 向设备中写
 */
static ssize_t mywdriver_write(struct file *filep, const char __user *buf, size_t count,
            loff_t *offset) {
    size_t avail;
    int ret;

    printk(KERN_INFO "MYW Driver: start write\n");

    avail = sizeof(mem) - *offset;

    memset(mem + *offset, 0, avail);

    /* 设备中剩余空间足够 */
    if (count > avail) {
        if (copy_from_user(mem + *offset, buf, avail) != 0) {
            return -EFAULT;
        }
        *offset += avail;
        ret = avail;

    } else { /* 设备中剩余空间不足 */
        if (copy_from_user(mem + *offset, buf, count) != 0) {
            return -EFAULT;
        }
        *offset += count;
        ret = count;
    }
    printk(KERN_INFO "MYW Driver: write %d B", ret);
    return ret;
}

/*
 * mywdriver_llseek - 设置文件的当前位置以进行读写
 */
static loff_t mywdriver_llseek(struct file *filep, loff_t off, int whence) {
    loff_t newpos;
    printk(KERN_INFO "MYW Driver: start llseek\n");

    switch (whence) {
    	case 0: /* SEEK_SET */
        	newpos = off;
        	break;
    	case 1: /* SEEK_CUR */
       	 	newpos = filep->f_pos + off;
       		break;
    	case 2: /* SEEK_END */
        	newpos = sizeof(mem) + off;
        	break;
    	default: /* Else: return error */
        	return -EINVAL;
    }

    if (newpos < 0) {
        return -EINVAL;
    }

    filep->f_pos = newpos;
    return newpos;
}

/*
 * 定义file_operations（能在设备上进行的操作函数指针集合）
 */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mywdriver_open,
    .release = mywdriver_release,
    .read = mywdriver_read,
    .llseek = mywdriver_llseek,
    .write = mywdriver_write,
};

/*
 * mywdriver_init - 模块加载函数
 */
static int __init mywdriver_init(void) {
    int ret;

    printk(KERN_INFO "Load module: mywdriver\n");

    devno = MKDEV(111, 0); // 生成设备号
    ret = register_chrdev_region(devno, 1, "mywdriver");
    // if failed
    if (ret < 0) {
    	printk(KERN_ALERT "Register the device failed of ret %d", ret);
        return ret;
    }

    /* 创建设备 */
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;

    cdev_add(&cdev, devno, 1); // 想系统添加该cdev

    /* 创建设备文件（省去手动mknod） */
    class = class_create(THIS_MODULE, "mywdriver");
    if (IS_ERR(class)) {
    	ret = PTR_ERR(class);
    	printk(KERN_ALERT "Create the class for device failed of ret %d", ret);
    	unregister_chrdev_region(devno, 1);
    	return ret;
    }
    device = device_create(class, NULL, devno, NULL, "mywdriver");
    if (IS_ERR(device)) {
    	class_destroy(class);
    	ret = PTR_ERR(device);
    	printk(KERN_ALERT "Create device file failed of ret %d", ret);
    	unregister_chrdev_region(devno, 1);
    	return ret;
    }

    return 0;
}

/*
 * mywdriver_cleanup - 模块卸载函数
 */
static void __exit mywdriver_cleanup(void) {
    printk(KERN_INFO "Cleanup module: mywdriver\n");

    /* 删除设备文件 */
    device_destroy(class, devno);
    class_unregister(class);
    class_destroy(class);

    unregister_chrdev_region(devno, 1);// 释放设备号 
    cdev_del(&cdev);// 注销cdev
}

module_init(mywdriver_init);
module_exit(mywdriver_cleanup);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DRIVER_NAME	"get_proc_info"
#define DRIVER_AUTHOR	"Nguyen Thanh Toan <thanhtoan1742@gmail.com>"
#define DRIVER_DESC	"a custom kernel to produce a custom char device that stream some process information"
#define DRIVER_LICENSE	"GPL"

#define DEV_NAME 	"get_proc_info"

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};
static int major;


struct proc_info {
	pid_t pid;
	char name[16];
};

struct procinfos {
	struct proc_info proc;
	struct proc_info parent_proc;
	struct proc_info oldest_child_proc;
};

long get_proc_info(pid_t pid, struct procinfos* proc) {
	struct task_struct* task;
	struct task_struct* parent_task;
	struct task_struct* child_task;
	struct task_struct* oldest_child_task;
	struct list_head* list;
	int error;


	task = current;
	if (pid > 0)
		task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (task == NULL)
		return 1;
	parent_task = task->parent;
	oldest_child_task = NULL;
	list_for_each(list, &task->children) {
		child_task = list_entry(list, struct task_struct, sibling);
		if (oldest_child_task == NULL || oldest_child_task->start_time > child_task->start_time)
			oldest_child_task = child_task;
	}


	if (proc != NULL) {
		error = 0;
		copy_to_user(&proc->proc.pid, &task->pid, sizeof(task->pid));
		copy_to_user(proc->proc.name, task->comm, 16);
		copy_to_user(&proc->parent_proc.pid, &parent_task->pid, sizeof(parent_task->pid));
		copy_to_user(proc->parent_proc.name, parent_task->comm, 16);
		copy_to_user(&proc->oldest_child_proc.pid, &oldest_child_task->pid, sizeof(oldest_child_task->pid));
		copy_to_user(proc->oldest_child_proc.name, oldest_child_task->comm, 16);
	}


	printk(KERN_INFO "caller pid, name: %d, %s\n", task->pid, task->comm);
	printk(KERN_INFO "caller's parent pid, name: %d, %s\n", parent_task->pid, parent_task->comm);
	if (oldest_child_task != NULL)
		printk(KERN_INFO "caller's oldest child task pid, name: %d, %s\n", oldest_child_task->pid, oldest_child_task->comm);
	list_for_each(list, &task->children) {
		child_task = list_entry(list, struct task_struct, sibling);
		printk(KERN_INFO "caller's child task pid, name: %d, %s\n", child_task->pid, child_task->comm);
	}
	return 0;
}

static int dev_open(struct inode* p_inode, struct file* p_file) {
	printk(KERN_INFO "opened %s\n", DEV_NAME);
	return 0;
}


static int dev_release(struct inode* p_inode, struct file* p_file) {
	printk(KERN_INFO "released %s\n", DEV_NAME);
	return 0;
}

static ssize_t dev_read(struct file* p_file, char* buffer, size_t len, loff_t* offset) {
	pid_t pid = -1;
	if (len > 0)
		pid = len;

	printk(KERN_INFO "%s pid: %d\n", DEV_NAME, pid);
	get_proc_info(pid, (struct procinfos*)(buffer));
	return 0;
}

static ssize_t dev_write(struct file* p_file, const char* buffer, size_t len, loff_t* offset) {
	printk(KERN_ALERT "%s is not supported for writing.", DEV_NAME);
	return -EFAULT;
}

static int __init init_custom_module(void) {
	major = register_chrdev(0, DEV_NAME, &fops);
	if (major < 0) {
		printk(KERN_ALERT "%s failed to load, major: %d\n", DRIVER_NAME, major);
		return major;
	}

	printk(KERN_INFO "%s loaded, major: %d\n", DRIVER_NAME, major);
	return 0;
}

static void __exit cleanup_custom_module(void) {
	unregister_chrdev(major, DEV_NAME);
	printk(KERN_INFO "%s unloaded\n", DRIVER_NAME);
}


module_init(init_custom_module);
module_exit(cleanup_custom_module);


MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

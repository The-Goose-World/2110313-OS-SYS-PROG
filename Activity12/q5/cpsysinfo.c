#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRERK PIROMSOPA, PH.D. <Krerk.P@chula.ac.th>");
MODULE_DESCRIPTION("\"CPSYSINFO\" CPSYSINFO Kernel Module");
static char *f_ptr;
static char *procinfo;
static char *meminfo;
#define DEVICENAME "cpsysinfo"
#define PROCFS_MAX_SIZE 1024
static int dev_major;
static int dev_open = 0;
static char msg[PROCFS_MAX_SIZE];
static int msg_len;
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static struct file_operations dev_fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release};

static void get_active_processes_info(void) {
    struct task_struct *task;
    msg_len = 0;
    for_each_process(task) {
        msg_len += snprintf(msg + msg_len, PROCFS_MAX_SIZE - msg_len,
                            "PID: %d, Name: %s\n", task->pid, task->comm);
    }
    f_ptr = msg_len;
}

static void get_memory_info(void) {
    struct sysinfo info;
    si_meminfo(&info);
    msg_len = snprintf(msg, PROCFS_MAX_SIZE,
                       "Total RAM: %lu KB\n", info.totalram);
    f_ptr = msg_len;
}

int init_module(void)
{
  printk(KERN_INFO "CPSYSINFO: init\n");
  printk(KERN_INFO "CPSYSINFO: dev CPSYSINFO init\n");
  dev_major = register_chrdev(0, DEVICENAME, &dev_fops);
  if (dev_major < 0)
  {
    printk(KERN_ALERT "CPSYSINFO\tFail register_chrdev CPSYSINFO with %d\n", dev_major);
    return dev_major;
  }
  printk(KERN_INFO "CPSYSINFO\tDevice MajorNumber %d.\n", dev_major);
  printk(KERN_INFO "CPSYSINFO\tTo create a device file:\n");
  printk(KERN_INFO "CPSYSINFO\t'mknod /dev/%s c %d 0'.\n", DEVICENAME, dev_major);
  printk(KERN_INFO "CPSYSINFO\tTry varying minor numbers.\n");
  printk(KERN_INFO "CPSYSINFO\tPlease remove the device file and module when done.\n");
  return 0;
}

static int device_open(struct inode *inode, struct file *file) {
    if (MINOR(inode->i_rdev) == 0) {
        get_active_processes_info();
    } else if (MINOR(inode->i_rdev) == 1) {
        get_memory_info();
    } else {
        return -EINVAL; // Invalid minor number
    }
    try_module_get(THIS_MODULE);
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "CPSYSINFO: cleanup\n");
}
static int device_release(struct inode *inode, struct file *file)
{
  dev_open--; /* We're now ready for our next caller */
  // release module
  module_put(THIS_MODULE);
  return 0;
}



static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
  int bytes_read = 0;
  if (*f_ptr == 0)
  {
    return 0;
  }
  while (length && *f_ptr)
  {
    put_user(*(f_ptr++), buffer++);
    length--;
    bytes_read++;
  }
  return bytes_read;
}

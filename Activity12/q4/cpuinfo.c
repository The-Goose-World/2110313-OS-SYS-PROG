// cpuinfo.c
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/fs.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRERK PIROMSOPA, PH.D. <Krerk.P@chula.ac.th>");
MODULE_DESCRIPTION("\"CPUINFO\" CPUINFO Kernel Module");
static char *f_ptr;
#define DEVICENAME "cpuinfo"
static int dev_major;
static int dev_open = 0;
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static struct file_operations dev_fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release};
// static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,unsigned int *ecx, unsigned int *edx)
// {
// /* ecx is often an input as well as an output. */
//   asm volatile("cpuid"
//   : "=a" (*eax),
//   "=b" (*ebx),
//   "=c" (*ecx),
//   "=d" (*edx)
//   : "0" (*eax), "2" (*ecx)
//   : "memory");
// }

int init_module(void)
{
  printk(KERN_INFO "CPUINFO: init\n");
  printk(KERN_INFO "CPUINFO: dev cpuinfo init\n");
  dev_major = register_chrdev(0, DEVICENAME, &dev_fops);
  if (dev_major < 0)
  {
    printk(KERN_ALERT "CPUINFO\tFail register_chrdev CPUINFO with %d\n", dev_major);
    return dev_major;
  }
  printk(KERN_INFO "CPUINFO\tDevice MajorNumber %d.\n", dev_major);
  printk(KERN_INFO "CPUINFO\tTo create a device file:\n");
  printk(KERN_INFO "CPUINFO\t'mknod /dev/%s c %d 0'.\n", DEVICENAME, dev_major);
  printk(KERN_INFO "CPUINFO\tTry varying minor numbers.\n");
  printk(KERN_INFO "CPUINFO\tPlease remove the device file and module when done.\n");
  return 0;
}

static int device_open(struct inode *inode, struct file *file)
{
  unsigned eax, ebx, ecx, edx;
  static char txt[200];
  static char vendorId[32];
  static char feature[128];
  // static char vendorId 
  eax = 0;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  sprintf(vendorId,"Vendor ID: %c%c%c%c%c%c%c%c%c%c%c%c\n", (ebx) & 0xFF, (ebx>>8) & 0xFF, (ebx>>16) & 0xFF, (ebx>>24) &0xFF, (edx) & 0xFF, (edx>>8) & 0xFF, (edx>>16) & 0xFF, (edx>>24) &0xFF, (ecx) & 0xFF, (ecx>>8) & 0xFF, (ecx>>16) & 0xFF, (ecx>>24) &0xFF);
  eax = 1;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  sprintf(feature,"stepping %d\nmodel %d\nfamily %d\nprocessor type %d\nextended model %d\nextended family %d\n", eax & 0xF, (eax >> 4) & 0xF, (eax >> 8) & 0xF, (eax >> 12) & 0x3, (eax >> 16) & 0xF, (eax >> 20) & 0xFF);
  eax = 3;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  sprintf(txt,"%s\n%s\nserial number 0x%08x%08x\n", vendorId, feature, edx, ecx);
  f_ptr = (char *)txt;
  try_module_get(THIS_MODULE);
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

void cleanup_module(void)
{
printk(KERN_INFO "CPUINFO: cleanup\n");
}
static int device_release(struct inode *inode, struct file *file)
{
  dev_open--; /* We're now ready for our next caller */
  // release module
  module_put(THIS_MODULE);
  return 0;
}
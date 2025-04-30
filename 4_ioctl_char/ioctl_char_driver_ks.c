#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "driver0"
#define MAJOR_NUM 240

#define IOCTL_RESET_COUNTER _IO('q', 1)

static int counter = 100;

// FILE OPERTIONS

static int devch_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device Open\n");
	return 0;
}

static int devch_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device closed\n");
	return 0;
}

static long devch_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case IOCTL_RESET_COUNTER:
			counter = 0;
			printk(KERN_INFO "Counter reset to 0 via IOCTL\n");
			break;
		default:
			return -EINVAL;
	}
	return 0;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = devch_open,
	.release = devch_release,
	.unlocked_ioctl = devch_ioctl,
};


static int __init simple_ioctl_init(void)
{
	int ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
	if(ret < 0)
	{
		printk(KERN_ERR "Failed to register character device\n");
		return ret;
	}

	printk(KERN_INFO "Simple IOCTL driver loaded\n");
	return 0;
}

static void __exit simple_ioctl_exit(void)
{
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk(KERN_INFO "Simple IOCTL driver unloaded\n");
}

module_init(simple_ioctl_init);
module_exit(simple_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devjit");
MODULE_DESCRIPTION("Simple character ioctl device driver");

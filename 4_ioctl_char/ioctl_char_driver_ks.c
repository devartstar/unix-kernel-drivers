#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "driver0"

#define IOCTL_RESET_COUNTER _IO('q', 1)

static int counter = 100;

static dev_t dev_num;
static struct cdev *my_cdev = NULL;

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
	int ret;
	
	// Get the major and minor number for the character device automatically
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if(ret < 0)
	{
		printk(KERN_ERR "Failed to register character device\n");
		return ret;
	}
	
	// Create and initialize the character device
	my_cdev = cdev_alloc();
	if(!my_cdev)
	{
		unregister_chrdev_region(dev_num, 1);
		printk(KERN_ERR "Failed to allocate memory to cdev\n");
		return -ENOMEM;
	}

	my_cdev->ops = &fops;
	ret = cdev_add(my_cdev, dev_num, 1);
	if(ret < 0)
	{
		unregister_chrdev_region(dev_num, 1);
		printk(KERN_ERR "Failed to add cdev to the system\n");
		return ret;
	}

	printk(KERN_INFO "Simple IOCTL driver loaded with major number %d\n", MAJOR(dev_num));
	return 0;
}

static void __exit simple_ioctl_exit(void)
{
	cdev_del(my_cdev);
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_INFO "Simple IOCTL driver unloaded\n");
}

module_init(simple_ioctl_init);
module_exit(simple_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devjit");
MODULE_DESCRIPTION("Simple character ioctl device driver");

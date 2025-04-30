#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "DEVCHdevice"


static int major;
static char message[256] = {0};
static short size_of_message;
static int number_opens = 0;

// function prototypes
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


// structure for file operations
static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

// Loading the module
static int __init my_driver_init(void) {
	
	printk(KERN_INFO "MyDevice: Initializing the MyDevice LKM\n");

	//
	// registering the device - 2 parts
	// 1. Allocating a device number (major + minor number)
	// 2. Register the device with kernel
	//
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		printk(KERN_ALERT "DEVCHdevcie failed to register a major number\n");
		return major;
	}

	printk(KERN_INFO "DEVCHdevice: registered successfully with major number %d\n", major);
	return 0;

}

// Unloading the module
static void __exit my_driver_exit(void) {
	
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "DEVCHdevice: unregistered successfully\n");

}

// When a process tries to open the device file
/**
 * Uerspace:
 * fd = open("/dev/DEVCHdevice", O_RDONLY);
 * LINUX kernel already handles file descriptor creation and mappings
 * dev_open() is jist a hook to implement custom logic
 */
static int dev_open(struct inode *pinode, struct file *pfile) {
	
	number_opens++;
	printk(KERN_INFO "DEVCHdevice: Device has been opened %d time(s)\n", number_opens);
	return 0;
}

// When a process tries to read form the device
/**
 * Userspace:
 * read(fd, user_buffer, 100);
 * kernel call the drivers dev_read()
 */
static ssize_t dev_read(struct file *pfile, char *buffer, size_t len, loff_t *offset) {
	
	if(*offset >= size_of_message) {
		printk(KERN_INFO "DEVCHdevice: offset = %lld, size_of_message = %d\n", *offset, size_of_message);
		return 0;
	}

	int err_count = 0;
	err_count = copy_to_user(buffer, message, size_of_message);

	if(err_count == 0) {
		printk(KERN_INFO "DEVCHdevice: Send %d characters to the user\n", size_of_message);
		*offset += size_of_message;
		return size_of_message;
	} else {
		printk(KERN_INFO "DEVCHdevice: Failed to send %d characters to the user\n", size_of_message);
		return -EFAULT; // bad address
	}

}

// When the process tries to write tot he device
static ssize_t dev_write(struct file *pfile, const char *buffer, size_t len, loff_t *offset) {
	
	copy_from_user(message, buffer, len);
	size_of_message = strlen(message);
	printk(KERN_INFO "DEVCHdevice: Recieved %zu characters from user\n", len);
	return len;

}


// When the process closes the device file
static int dev_release(struct inode *pinode, struct file *pfile) {

	printk(KERN_INFO "DEVCHdevice: Device successfully closed\n");
	return 0;

}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devjit Choudhury");
MODULE_DESCRIPTION("A simple character driver");
MODULE_VERSION("0.1");

module_init(my_driver_init);
module_exit(my_driver_exit);

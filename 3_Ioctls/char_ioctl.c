#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "DEVCHdevice"
#define BUF_LEN 1024

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

static char device_buffer[BUF_LEN];
// how much data is written
static int buffer_size = 0;
// to track the read position
static int read_position = 0;

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

	int bytes_to_copy;
	int error_count;
	
	// check the bytes of data from current position left to read
	bytes_to_copy = buffer_size - read_position;

	if(bytes_to_copy <= 0) {
		printk(KERN_INFO "DEVCHdevice: No more data left to read");
		return 0;
	}
	
	// User asked for less bytes
	if(len < bytes_to_copy) {
		bytes_to_copy = len;
	}

	error_count = copy_to_user(buffer, device_buffer + read_position, bytes_to_copy);
	// Copy data from from the buffer starting from `read_position`
	// Copy data form device_buffer (kernelspace) to buffer (userspace)
	if(error_count != 0) {
		 printk(KERN_INFO "DEVCHdevice: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;
	}

	printk(KERN_INFO "DEVCHdevice: Sent %d bytes to the user\n", bytes_to_copy);

	// Update the read position
	read_position += bytes_to_copy;
	printk(KERN_DEBUG "DEVCHdevice: Current read position %d of buffer size %d\n", read_position, buffer_size);

	if(read_position >= buffer_size) {
		printk(KERN_DEBUG "DEVCHdevice clearing the buffer\n");
		// Clear the buffer
		memset(device_buffer, 0, sizeof(device_buffer));
		read_position = 0;
		buffer_size = 0;
	}

	return bytes_to_copy;

}

// When the process tries to write tot he device
static ssize_t dev_write(struct file *pfile, const char __user *buffer, size_t len, loff_t *offset) {

	if(len > BUF_LEN) {
		printk(KERN_WARNING "DEVCHdevice: Not enough space in device buffer\n");
		return -ENOMEM;
	}
	
	// copy from userspace ot kernel space
	if(copy_from_user(device_buffer, buffer, len)) {
		return -EFAULT;
	}

	buffer_size = len;
	device_buffer[len] = '\0';

	printk(KERN_INFO "DEVCHdevice: Recieved %zu characters from user: %s\n", len, device_buffer);

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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devjit Choudhury");
MODULE_DESCRIPTION("A simple Linux Kernel Module");
MODULE_VERSION("0.1");

/**
 * __init : tells kernel to free the function from memory after init is done
 */
static int __init my_driver_init(void) {
	printk(KERN_INFO "Hello, Kernel! Driver Loaded\n");
	return 0;
}

/**
 * __exit : tells kerne; that function used only during unloadin
 */
static void __exit my_driver_exit(void) {
		printk(KERN_INFO "Goodbye, Kernel! Driver Unloaded\n");
}


module_init(my_driver_init);
module_exit(my_driver_exit);




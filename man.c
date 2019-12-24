#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "man"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("l4 <l4.foss@gmail.com>");
MODULE_DESCRIPTION("A trivial kernel module");
MODULE_VERSION("0.1");

/*
 * static variables
 */
static char message[256];
static bool man_is_open;
static int man_major;
static struct class *man_class;
static struct device *man_dev;

/*
 * some internal function prototypes
 */

/*
 * fops
 */
static int man_sysfs_open(struct inode *, struct file *);
static ssize_t man_sysfs_read(struct file *, char *, size_t, loff_t *);
static ssize_t man_sysfs_write(struct file *, const char *, size_t, loff_t *);
static int man_sysfs_release(struct inode *, struct file *);

/*
 * constructor/destructor
 */
static int __init man_init(void);
static void __exit man_exit(void);

/*
 * file operations
 */
static struct file_operations fops = {
	.open = man_sysfs_open,
	.read = man_sysfs_read,
	.write = man_sysfs_write,
	.release = man_sysfs_release
};

static int man_sysfs_open(struct inode *i, struct file *f) {
	printk(KERN_INFO "mlamp: Device got opened!\n");
	return 0;
}

static ssize_t man_sysfs_read(struct file *f, char *buf, size_t len, 
		loff_t *offset) {
	int err = 0;

	err = copy_to_user(buf, message, sizeof(message));

	if (err == 0) {
		return 0;
	}
	else {
		printk(KERN_ERR "Error sending to user\n");
		return -EFAULT;
	}
}

static ssize_t man_sysfs_write(struct file *f, const char *buf, size_t len, loff_t *offset) {
	return 0;
}

static int man_sysfs_release(struct inode *inode, struct file *f) {
	return 0;
}

static int __init man_init(void) {
	printk(KERN_INFO "mlamp: loading ...\n");
	man_is_open = false;

	man_major = register_chrdev(0, DEVICE_NAME, &fops);
	if (man_major < 0) {
		printk(KERN_ALERT "mlamp: error registering driver\n");
		return -1;
	}

	man_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(man_class)) {
		unregister_chrdev(man_major, DEVICE_NAME);
		printk(KERN_ALERT "mlamp: failed to register device class");
		return PTR_ERR(man_class);
	}

	man_dev = device_create(man_class, NULL, MKDEV(man_major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(man_dev)) {
		class_destroy(man_class);
		unregister_chrdev(man_major, DEVICE_NAME);
		printk(KERN_ALERT "mlamp: failed to create device\n");
		return PTR_ERR(man_dev);
	}

	printk(KERN_INFO "mlamp: registered with major: %d\n", man_major);
	return 0;
}

static void __exit man_exit(void) {
	printk(KERN_INFO "mlamp: exiting ...\n");
	device_destroy(man_class, MKDEV(man_major, 0));
	class_destroy(man_class);
	unregister_chrdev(man_major, DEVICE_NAME);
	printk(KERN_INFO "mlamp: done\n");

}

module_init(man_init);
module_exit(man_exit);

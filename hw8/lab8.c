#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

// global data (assuming only one process opens the device at a time)
static unsigned long open_jiffies; // save jiffies at open time
static char message_buffer[100];   // buffer to store our response message
static int message_ready = 0;      // flag to track if message is ready
static int message_len = 0;        // length of the message

// this gets called when someone opens our device
static int my_open(struct inode *inode, struct file *file)
{
    // just record the current jiffies when device is opened
    open_jiffies = jiffies;
    message_ready = 0; // reset message state

    printk(KERN_INFO "lab8: device opened, jiffies = %lu\n", open_jiffies);
    return 0; // success
}

// this gets called when someone reads from our device
static ssize_t my_read(struct file *file, char __user *buf,
                       size_t count, loff_t *pos)
{
    unsigned long diff;
    int bytes_to_copy;

    // first time reading, generate the message
    if (!message_ready)
    {
        // calculate time diff
        diff = jiffies - open_jiffies;

        // format time diff and process ID into our buffer
        message_len = snprintf(message_buffer, sizeof(message_buffer),
                               "Time elapsed: %lu ms\nProcess ID: %d\n",
                               diff * 1000 / HZ, current->pid);

        message_ready = 1; // message is now ready
    }

    // check if we're at EOF
    if (*pos >= message_len)
        return 0; // indicate EOF by returning 0

    // calculate how many bytes to actually copy
    bytes_to_copy = min(count, (size_t)(message_len - *pos));

    // copy data to user space
    if (copy_to_user(buf, message_buffer + *pos, bytes_to_copy))
    {
        printk(KERN_ERR "lab8: failed to copy data to user\n");
        return -EFAULT; // bad address
    }

    // update position
    *pos += bytes_to_copy;

    // return how many bytes we sent
    return bytes_to_copy;
}

// setup the file operations
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
};

// setup the misc device
static struct miscdevice my_dev = {
    .minor = MISC_DYNAMIC_MINOR, // let kernel assign minor number
    .name = "lab8",              // device will be at /dev/lab8
    .fops = &my_fops,            // file operations
};

// init function - called when module is loaded
static int __init lab8_init(void)
{
    int ret;

    // register with misc subsystem
    ret = misc_register(&my_dev);
    if (ret)
    {
        printk(KERN_ERR "lab8: failed to register as misc device\n");
        return ret;
    }

    printk(KERN_INFO "lab8: module loaded successfully\n");
    return 0; // success
}

// exit function - called when module is unloaded
static void __exit lab8_exit(void)
{
    // unregister from misc subsystem
    misc_deregister(&my_dev);
    printk(KERN_INFO "lab8: module unloaded\n");
}

// register init and exit functions
module_init(lab8_init);
module_exit(lab8_exit);
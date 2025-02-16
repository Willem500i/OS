#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>

MODULE_LICENSE("Dual BSD/GPL");

static unsigned long jiff_starting_val;
static ktime_t start_time;

static int hello_init(void)
{
    printk(KERN_ALERT "Hello, world\n");

    jiff_starting_val = jiffies;

    start_time = ktime_get_boottime();

    // print tick time in ms
    printk(KERN_ALERT "jiffies: %lu\n", jiff_starting_val);

    return 0;
}
static void hello_exit(void)
{
    // Using the difference in the value of jiffies from inserting the module to removing the module
    unsigned long j_diff = jiffies - jiff_starting_val;
    unsigned long t_diff = ktime_get_boottime() - start_time;

    printk(KERN_ALERT "jiffies diff: %lu\n", j_diff);
    printk(KERN_ALERT "time diff: %lu\n", t_diff);

    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

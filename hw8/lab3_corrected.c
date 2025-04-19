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

    // Print current jiffies value
    printk(KERN_ALERT "jiffies: %lu\n", jiff_starting_val);

    // Print tick interval in milliseconds
    printk(KERN_ALERT "Tick interval (ms): %d\n", (int)(1000 / HZ));

    return 0;
}

static void hello_exit(void)
{
    // Calculate jiffies difference
    unsigned long j_diff = jiffies - jiff_starting_val;

    // Convert jiffies difference to milliseconds
    unsigned long j_diff_ms = j_diff * 1000 / HZ;

    // Get current time and calculate time difference in nanoseconds
    ktime_t end_time = ktime_get_boottime();
    s64 t_diff_ns = ktime_to_ns(ktime_sub(end_time, start_time));

    // Convert nanoseconds to milliseconds
    s64 t_diff_ms = t_diff_ns / 1000000;

    printk(KERN_ALERT "jiffies diff: %lu\n", j_diff);
    printk(KERN_ALERT "elapsed time using jiffies (ms): %lu\n", j_diff_ms);
    printk(KERN_ALERT "elapsed time using ktime (ms): %lld\n", t_diff_ms);

    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
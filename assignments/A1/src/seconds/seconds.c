/**
 * Lab 1 - seconds kernel module - outputs the number of seconds since the module is loaded
 * Group 12
 * Yash Bhatia - 400362372
 * Khawja Labib - 400356836
 * 30th January 2026
 */

/**
 * lsb_release -a command
 * Distributor ID: Ubuntu
 * Description:    Ubuntu 25.10
 * Release:        25.10
 * Codename:       questing
 */

 /**
  * uname -r command
  * 6.17.0-8-generic
  */


/**
 * seconds.c
 * our seconds.c program was built on top of the newKernel.c code sample
 *
 * Kernel module that communicates with /proc file system.
 * 
 * Samkith jain September 6, 2024
 * Comp Sci 3SH3, Fall 2024
 * Reference:  hello.c Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h> // importing jiffies
#include <linux/param.h> // importing param.h to use HZ

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static const struct proc_ops my_proc_ops = {
        .proc_read = proc_read,
};

unsigned long g_load_tick; // for seconds.c - initialize start time variable

/* This function is called when the module is loaded. */
int proc_init(void)
{

        // creates the /proc/seconds entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &my_proc_ops);

        g_load_tick = jiffies; // for seconds.c - the tick at which the module is inserted

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        // removes the /proc/seconds entry
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/seconds is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 * 
 * for seconds.c
 * here we calculate the time elapsed as the difference between the tick -
 * at which cat /proc/seconds is called and the g_load_tick devided by the kernel HZ
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;
        unsigned long time_elapsed; // for seconds.c - time elapsed var

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
        time_elapsed = (jiffies - g_load_tick)/HZ; // for seconds.c - time elapsed calculation 

        rv = sprintf(buffer, "Time elapsed since module inserted %ul\n", time_elapsed); // for seconds.c - load the time elapsed to the buffer

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("seconds Module");
MODULE_AUTHOR("G12");


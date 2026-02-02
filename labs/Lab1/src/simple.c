/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

/* Print the PCB information for 'init': used in Simple Init */
void print_init_pcb(void)
{
	struct task_struct *temp = &init_task; //get the init task
	
	printk(KERN_INFO "init_task pid:%d\n", temp->pid);

	printk(KERN_INFO "init_task flags:%d\n", temp->flags);
	printk(KERN_INFO "init_task runtime priority:%d\n", temp->rt_priority);
	printk(KERN_INFO "init_task process policy:%d\n", temp->policy);
	printk(KERN_INFO "init_task task group id:%d\n", temp->tgid);

}
/* This function is called when the module is loaded. */
static int simple_init(void)
{
       printk(KERN_INFO "Loading Module\n");
	print_init_pcb();

       return 0;
}

/* This function is called when the module is removed. */
static void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");


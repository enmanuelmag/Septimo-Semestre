// Defining __KERNEL__ and MODULE allows us to access kernel-level code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
 
#undef MODULE
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

    int init_module(void)
    {
    printk("Hello Kernel World");
    return 0;
    }
   
    void cleanup_module(void)
    {
    printk(KERN_INFO "Cleaning Up.\n");
    }


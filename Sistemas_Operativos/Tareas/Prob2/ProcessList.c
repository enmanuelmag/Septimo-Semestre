// Defining __KERNEL__ and MODULE allows us to access kernel-level code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
 
#undef MODULE
#define MODULE

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

    int init_module(void)
    {
    struct task_struct *task;
    
    int a = 0;

    while(a < 60){
        long process_count = 0;
        
        for_each_process(task)
        {
            process_count++;
        }
        printk("%ld", process_count);
        msleep(2000);
        a++;
    }
   
    return 0;
    }
   
    void cleanup_module(void)
    {
    printk(KERN_INFO "Cleaning Up.\n");
    }


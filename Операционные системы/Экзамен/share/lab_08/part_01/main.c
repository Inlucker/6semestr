#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define IRQ 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pronin Arseny");

static int my_dev_id;
struct tasklet_struct *my_tasklet;
char *my_tasklet_data = "my_tasklet_function was called";

void my_tasklet_function(unsigned long data)
{
    printk(KERN_INFO "my_tasklet: State: %ld, Count: %ld, Data: %s", my_tasklet->state, my_tasklet->count, my_tasklet->data);
}

irqreturn_t my_handler(int irq, void *dev)
{
    if (irq == IRQ)
    {
        tasklet_schedule(my_tasklet);
        printk("my_tasklet: Tasklet scheduled\n");
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}


static int __init my_tasklet_init(void)
{
    my_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);

    if (!my_tasklet)
    {
        printk(KERN_INFO "my_tasklet: kmalloc error!");
        return -1;
    }
    tasklet_init(my_tasklet, my_tasklet_function, (unsigned long)my_tasklet_data);

    int res = request_irq(IRQ, my_handler, IRQF_SHARED, "my_irq_handler", &my_dev_id);

    if (res)
        printk(KERN_ERR "my_tasklet: cannot register my_handler\n");
    else
        printk(KERN_INFO "my_tasklet: module loaded\n");

    return res;
}

static void __exit my_tasklet_exit(void)
{
    synchronize_irq(IRQ);
    tasklet_kill(my_tasklet);
    kfree(my_tasklet);
    free_irq(IRQ, &my_dev_id);

    printk(KERN_DEBUG "my_tasklet: module unloaded\n");
}

module_init(my_tasklet_init)
module_exit(my_tasklet_exit)



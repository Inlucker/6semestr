#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <asm/io.h>

#define SHARED_IRQ 1

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Pronin Arseny");

static int my_dev_id;
static int count = 0;

struct workqueue_struct *my_wq;

char *simbol;

void my_wq_function(struct work_struct *work)
{
    int code = inb(0x60);
    char * ascii[84] =
    {" ", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Backspace",
     "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl",
     "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "\"", "'", "Shift (left)", "|",
     "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift (right)",
     "*", "Alt", "Space", "CapsLock",
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
     "NumLock", "ScrollLock", "Home", "Up", "Page-Up", "-", "Left",
     " ", "Right", "+", "End", "Down", "Page-Down", "Insert", "Delete"};

    if (code < 84)
    {
        simbol = ascii[code];
        printk(KERN_INFO "my_workqueue: key pressed = %s\n", ascii[code]);
    }
}

void my_wq_function2(struct work_struct *work)
{
    count += 2;
    printk(KERN_INFO "my_workqueue2: counter %d\n", count);
    printk(KERN_INFO "my_workqueue2: sleep\n");
    msleep(100);
    printk(KERN_INFO "my_workqueue2: wake up\n");
}

DECLARE_WORK(my_workqueue, my_wq_function);
DECLARE_WORK(my_workqueue2, my_wq_function2);

static irqreturn_t my_handler(int irq, void *dev_id)
{
    if (irq == SHARED_IRQ)
    {
        queue_work(my_wq, &my_workqueue);
        queue_work(my_wq, &my_workqueue2);
        printk(KERN_INFO "my_workqueue in my_handler\n");
        return IRQ_HANDLED;
    }
    else
        return IRQ_NONE;
}

static int __init my_workqueue_init(void)
{
    if (request_irq(SHARED_IRQ, my_handler, IRQF_SHARED, "my_handler", &my_dev_id))
    {
        printk(KERN_ERR "Can't register handler\n");
        return -1;
    }

    my_wq = create_workqueue("workqueue");
    if (my_wq)
        printk(KERN_INFO "my_workqueue created!\n");
    else
    {
        free_irq(SHARED_IRQ, &my_dev_id);
        printk(KERN_ERR "Can't create my_workqueue\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "my_workqueue Module loaded!\n");

    return 0;
}

static void __exit my_workqueue_exit(void)
{
    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);
    free_irq(SHARED_IRQ, &my_dev_id);
    printk(KERN_INFO "my_workqueue Module unloaded\n");
}

module_init(my_workqueue_init)
module_exit(my_workqueue_exit)

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pronin Arseny");

static int __init md_init(void)
{
    struct task_struct *task = &init_task;

    do
    {
        printk(KERN_INFO " + pid=%d, name=%s, state=%lu, prio=%d, ppid=%d, p_name=%s\n",
               task->pid, task->comm,
               task->state, task->prio,
               task->parent->pid, task->parent->comm);

    } while ((task = next_task(task)) != &init_task);

    printk(KERN_INFO " + current: pid=%d, name=%s, state=%lu, prio=%d, ppid=%d, p_name=%s\n",
           current->pid, current->comm,
           current->state, current->prio,
           current->parent->pid, current->parent->comm);

    return 0;
}

static void __exit md_exit(void)
{
    printk(KERN_INFO " + lsproc module unloaded!\n");
}

module_init(md_init);
module_exit(md_exit);

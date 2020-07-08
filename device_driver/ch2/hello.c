#include <linux/kernel.h> /* pr_info 所需 include 檔案*/
#include <linux/init.h>
#include <linux/module.h> /* 所有 module 需要檔案*/
#include <linux/version.h>
#include <linux/sched.h>  /* current (process info) requires this */

MODULE_DESCRIPTION("Hello World !!");
MODULE_AUTHOR("Bo-Yi Wu <appleboy.tw at="" gmail.com="">");
MODULE_LICENSE("GPL");

static int __init hello_init(void)
//static int hello_init(void)
{
    pr_info("Hello, world\n");
    pr_info("The process is \"%s\" (pid %i)\n", current->comm, current->pid);
    printk(KERN_ALERT "alert: hello world~~~~~~~~~~~~~~~~\n");
                   //^ There is no "," character; This is the mistake beginner may make
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "hello Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);


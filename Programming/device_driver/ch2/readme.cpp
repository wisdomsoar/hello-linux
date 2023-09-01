
0. Set root passward:
   sudo passwd root

1. Change to root
   su -

2. Check linux kernel version:
   uname -r 
  
3. mkdir -p /usr/src/linux-headers-5.4.0-31-generic/hello
   cd hello

4. vi hello.c

-------------------------------------------------------------------------------------------------------
#include <linux/kernel.h> /* pr_info 所需 include 檔案*/
#include <linux/init.h>
#include <linux/module.h> /* 所有 module 需要檔案*/
#include <linux/version.h>
#include <linux/sched.h>  /* current (process info) requires this */

MODULE_DESCRIPTION("Hello World !!");
MODULE_AUTHOR("Bo-Yi Wu");
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

-------------------------------------------------------------------------------------------------------

5. vi Makefile

-------------------------------------------------------------------------------------------------------
#
# Makefile by appleboy
#
obj-m       += hello.o
KVERSION := $(shell uname -r)

all:
#shell command: make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
#-C: change working directory to find makefile (?)
#M: come back hello folder and build module target
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
#^^^
##Makefile must use <tab> to do indent

clean:
        $(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) 

-------------------------------------------------------------------------------------------------------

6. make 

7. insmod hello.ko

8. dmesg | tail
   See the result


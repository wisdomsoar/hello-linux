
#include <linux/kernel.h>       /* for pr_info */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/init.h>
#include <linux/module.h> 
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/fs.h>           /* for get device id, ... */  /* file_operations */ 
#include <linux/cdev.h>         /* struct cdev */
#include <linux/types.h>	/* dev_t, size_t */
#include <linux/kdev_t.h>       /* MAJOR, MINOR MKDEV */
#include <linux/uaccess.h>      /* for copy_to_user */

//#define USE_PIPE
//#define USE_ACCESS
//#define USE_IOCTL
#define USE_FILE_OPERATION 1


MODULE_DESCRIPTION("Test alloc_chrdev_region");
MODULE_AUTHOR("Chihyang");
MODULE_LICENSE("GPL");


/*      ------------
       |            |   .---> -----------     .---> ----------- 
       |scull_device|   |    | scull_qst |    |    | scull_qst |
       |   Data ----|---'    |  Next-----|----'    |  Next-----|-------->(end of list)
        ------------         |  Data-----|----.    |  Data-----|----.  
                              -----------     |     -----------     |
       .--------------------------------------'                     |
       |                           .--------------------------------'
       |                   ___     |                   ___
       |               __>|___|    |               __>|___|
       '--> ------- __|    ___     '->  ------- __|    ___                    
           |-------|_____>|___|        |-------|_____>|___| 
           |-------|  ...  ___         |-------|  ...  ___
           |  ...  |_____>|___|        |  ...  |_____>|___|
            -------                     ------- 
*/ 

#ifdef USE_IOCTL

/* Use 'k' as magic number */
#define SCULL_IOC_MAGIC  'k'
/* Please use a different 8-bit number in your code */

#define SCULL_IOCRESET    _IO(SCULL_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 */
#define SCULL_IOCSQUANTUM _IOW(SCULL_IOC_MAGIC,  1, int)
#define SCULL_IOCSQSET    _IOW(SCULL_IOC_MAGIC,  2, int)
#define SCULL_IOCTQUANTUM _IO(SCULL_IOC_MAGIC,   3)
#define SCULL_IOCTQSET    _IO(SCULL_IOC_MAGIC,   4)
#define SCULL_IOCGQUANTUM _IOR(SCULL_IOC_MAGIC,  5, int)
#define SCULL_IOCGQSET    _IOR(SCULL_IOC_MAGIC,  6, int)
#define SCULL_IOCQQUANTUM _IO(SCULL_IOC_MAGIC,   7)
#define SCULL_IOCQQSET    _IO(SCULL_IOC_MAGIC,   8)
#define SCULL_IOCXQUANTUM _IOWR(SCULL_IOC_MAGIC, 9, int)
#define SCULL_IOCXQSET    _IOWR(SCULL_IOC_MAGIC,10, int)
#define SCULL_IOCHQUANTUM _IO(SCULL_IOC_MAGIC,  11)
#define SCULL_IOCHQSET    _IO(SCULL_IOC_MAGIC,  12)

#endif


#define SCULL_NR_DEVS 4    /* scull0 through scull3 */

#ifdef USE_PIPE
#define SCULL_P_NR_DEVS 4  /* scullpipe0 through scullpipe3 */

struct scull_pipe {
        wait_queue_head_t inq, outq;       /* read and write queues */
        char *buffer, *end;                /* begin of buf, end of buf */
        int buffersize;                    /* used in pointer arithmetic */
        char *rp, *wp;                     /* where to read, where to write */
        int nreaders, nwriters;            /* number of openings for r/w */
        struct fasync_struct *async_queue; /* asynchronous readers */
        struct semaphore sem;              /* mutual exclusion semaphore */
        struct cdev cdev;                  /* Char device structure */
};

static int scull_p_nr_devs = SCULL_P_NR_DEVS;	/* number of pipe devices */
static struct scull_pipe *scull_p_devices;
dev_t scull_p_devno;			/* Our first device number */

/*
 * Set up a cdev entry.
 */
static void scull_p_setup_cdev(struct scull_pipe *dev, int index)
{
	int err, devno = scull_p_devno + index;
    
	cdev_init(&dev->cdev, &scull_pipe_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scullpipe%d", err, index);
}


/*
 * Initialize the pipe devs; return how many we did.
 */
int scull_p_init(dev_t firstdev)
{
        int i, result;

        result = register_chrdev_region(firstdev, scull_p_nr_devs, "scullp");
        if (result < 0) {
                printk(KERN_NOTICE "Unable to get scullp region, error %d\n", result);
                return 0;
        }
        scull_p_devno = firstdev;
        scull_p_devices = kmalloc(scull_p_nr_devs * sizeof(struct scull_pipe), GFP_KERNEL);
        if (scull_p_devices == NULL) {
                unregister_chrdev_region(firstdev, scull_p_nr_devs);
                return 0;
        }
        memset(scull_p_devices, 0, scull_p_nr_devs * sizeof(struct scull_pipe));
        for (i = 0; i < scull_p_nr_devs; i++) {
                init_waitqueue_head(&(scull_p_devices[i].inq));
                init_waitqueue_head(&(scull_p_devices[i].outq));
		//ref: http://albert-oma.blogspot.com/2012/08/linux-ubuntu-skull.html
		sema_init(&scull_devices[i].sem, 1);
                //init_MUTEX(&scull_p_devices[i].sem);
                scull_p_setup_cdev(scull_p_devices + i, i);
        }
#ifdef SCULL_DEBUG
        create_proc_read_entry("scullpipe", 0, NULL, scull_read_p_mem, NULL);
#endif
        return scull_p_nr_devs;
}

/*
 * This is called by cleanup_module or on failure.
 * It is required to never fail, even if nothing was initialized first
 */
void scull_p_cleanup(void)
{
	int i;

#ifdef SCULL_DEBUG
	remove_proc_entry("scullpipe", NULL);
#endif

	if (!scull_p_devices)
		return; /* nothing else to release */

	for (i = 0; i < scull_p_nr_devs; i++) {
		cdev_del(&scull_p_devices[i].cdev);
		kfree(scull_p_devices[i].buffer);
	}
	kfree(scull_p_devices);
	unregister_chrdev_region(scull_p_devno, scull_p_nr_devs);
	scull_p_devices = NULL; /* pedantic */
}

#endif //USE_PIPE

/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "scull_dev->data" points to an array of pointers, each
 * pointer refers to a memory area of SCULL_QUANTUM bytes.
 *
 * The array (quantum-set) is SCULL_QSET long.
 */
#define SCULL_QUANTUM 4000
#define SCULL_QSET    1000
int scull_major = 0;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;

/*
 * Representation of scull quantum sets.
 */
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;  /* Pointer to first quantum set */
	int quantum;              /* the current quantum size */
	int qset;                 /* the current array size */
	unsigned long size;       /* amount of data stored here */
	unsigned int access_key;  /* used by sculluid and scullpriv */
	struct semaphore sem;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure          */ /* The interface between device and kernel  */
};

struct scull_dev *scull_devices;	/* allocated in scull_init_module */

#ifdef USE_FILE_OPERATION

/*
 * The "extended" operations -- only seek
 */

loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
	struct scull_dev *dev = filp->private_data;
	loff_t newpos;

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = off;
		break;

	  case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

	  case 2: /* SEEK_END */
		newpos = dev->size + off;
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}


/*
 * Follow the list
 */
struct scull_qset *scull_follow(struct scull_dev *dev, int n)
{
	struct scull_qset *qs = dev->data;

        /* Allocate first qset explicitly if need be */
	if (! qs) {
		qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (qs == NULL)
			return NULL;  /* Never mind */
		memset(qs, 0, sizeof(struct scull_qset));
	}

	/* Then follow the list */
	while (n--) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if (qs->next == NULL)
				return NULL;  /* Never mind */
			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
		continue;
	}
	return qs;
}


/*
 * Data management: read and write
 */

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data; 
	struct scull_qset *dptr;	/* the first listitem */
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset; /* how many bytes in the listitem */
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	if (*f_pos >= dev->size)
		goto out;
	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	/* find listitem, qset index, and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position (defined elsewhere) */
	dptr = scull_follow(dev, item);

	if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
		goto out; /* don't fill holes */

	/* read only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

  out:
	up(&dev->sem);
	return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/* find listitem, qset index and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position */
	dptr = scull_follow(dev, item);
	if (dptr == NULL)
		goto out;
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if (!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto out;
	}
	/* write only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

        /* update the size */
	if (dev->size < *f_pos)
		dev->size = *f_pos;

  out:
	up(&dev->sem);
	return retval;
}

#ifdef USE_IOCTL

/*
 * The ioctl() implementation
 */

int scull_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

	int err = 0, tmp;
	int retval = 0;
    
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch(cmd) {

	  case SCULL_IOCRESET:
		scull_quantum = SCULL_QUANTUM;
		scull_qset = SCULL_QSET;
		break;
        
	  case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_quantum = arg;
		break;

	  case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
		retval = __put_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
		return scull_quantum;

	  case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		retval = __get_user(scull_quantum, (int __user *)arg);
		if (retval == 0)
			retval = __put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		scull_quantum = arg;
		return tmp;
        
	  case SCULL_IOCSQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCTQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_qset = arg;
		break;

	  case SCULL_IOCGQSET:
		retval = __put_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCQQSET:
		return scull_qset;

	  case SCULL_IOCXQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		retval = __get_user(scull_qset, (int __user *)arg);
		if (retval == 0)
			retval = put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		scull_qset = arg;
		return tmp;

        /*
         * The following two change the buffer size for scullpipe.
         * The scullpipe device uses this same ioctl method, just to
         * write less code. Actually, it's the same driver, isn't it?
         */

	  case SCULL_P_IOCTSIZE:
		scull_p_buffer = arg;
		break;

	  case SCULL_P_IOCQSIZE:
		return scull_p_buffer;


	  default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}
	return retval;

}
#endif //USE_IOCTL


/*
 * Empty out the scull device; must be called with the device
 * semaphore held.
 */
int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}

/*
 * Open and close
 */

int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev; /* device information */

	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev; /* for other methods */

	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (down_interruptible(&dev->sem))
			return -ERESTARTSYS;
		scull_trim(dev); /* ignore errors */
		up(&dev->sem);
	}
	return 0;          /* success */
}

int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}


struct file_operations scull_fops = {
	.owner =    THIS_MODULE,
	.llseek =   scull_llseek,
	.read =     scull_read,
	.write =    scull_write,
#ifdef USE_IOCTL
	.ioctl =    scull_ioctl,
#endif
	.open =     scull_open,
	.release =  scull_release,
};
#endif


#if USE_ACCESS
static dev_t scull_a_firstdev;  /* Where our range begins */
#endif



static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err, devno = MKDEV(scull_major, scull_minor + index);

#if 0
	struct cdev * my_cdev = cdev_alloc();
	my_cdev->ops = &my_fops;
#endif

    
#ifdef USE_FILE_OPERATION
	cdev_init(&dev->cdev, &scull_fops);
#endif

	dev->cdev.owner = THIS_MODULE;

#ifdef USE_FILE_OPERATION
	dev->cdev.ops = &scull_fops;
#endif
        /* add to kernel... */
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}



#ifdef USE_ACCESS
int scull_access_init(dev_t firstdev)
{
        int result, i;

        /* Get our number space */
        result = register_chrdev_region (firstdev, SCULL_N_ADEVS, "sculla");
        if (result < 0) {
                printk(KERN_WARNING "sculla: device number registration failed\n");
                return 0;
        }
        scull_a_firstdev = firstdev;

        /* Set up each device. */
        for (i = 0; i < SCULL_N_ADEVS; i++)
                scull_access_setup (firstdev + i, scull_access_devs + i);
        return SCULL_N_ADEVS;
}
#endif //USE_ACCESS



/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void scull_cleanup_module(void)
{
	printk(KERN_ALERT "scull: Goodbye...\n");
	int i;
	dev_t devno = MKDEV(scull_major, scull_minor);

	/* Get rid of our char dev entries */
	if (scull_devices) {
		for (i = 0; i < scull_nr_devs; i++) {
			scull_trim(scull_devices + i);
			cdev_del(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

#ifdef SCULL_DEBUG /* use proc only if debugging */
	scull_remove_proc();
#endif

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, scull_nr_devs);

	/* and call the cleanup functions for friend devices */
#ifdef USE_PIPE
	scull_p_cleanup();
#endif
#ifdef USE_ACCESS
	scull_access_cleanup();
#endif

}

static int __init scull_init_module(void)
{
    int result, i;
    dev_t dev = 0;

    pr_info("The process is %s (pid %i)\n", current->comm, current->pid);
    printk(KERN_ALERT "alert: scull...\n");
    
    result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
				"scull");
    scull_major = MAJOR(dev);

    if (result < 0) {
	printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
	return result;
    }

        /* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
    scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
    if (!scull_devices) {
	result = -ENOMEM;
	goto fail;  /* Make this more graceful */
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

	/* Initialize each device. */
    for (i = 0; i < scull_nr_devs; i++) 
    {
        scull_devices[i].quantum = scull_quantum;
	scull_devices[i].qset = scull_qset;
	//ref: http://albert-oma.blogspot.com/2012/08/linux-ubuntu-skull.html
	sema_init(&scull_devices[i].sem, 1);
	//init_MUTEX(&scull_devices[i].sem);
	scull_setup_cdev(&scull_devices[i], i);
    }

    /* At this point call the init function for any friend device */
    dev = MKDEV(scull_major, scull_minor + scull_nr_devs);

#ifdef USE_PIPE
    dev += scull_p_init(dev);
#endif

#ifdef USE_ACCESS
    dev += scull_access_init(dev);
#endif
    return 0;

fail:
    scull_cleanup_module();
    return result;

}
 


 

module_init(scull_init_module);
module_exit(scull_cleanup_module);




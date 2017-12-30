#include <linux/init.h>  //初始换函数
#include <linux/kernel.h>  //内核头文件
#include <linux/module.h>  //模块的头文件

#include <linux/fs.h>

#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
//下面这三个头文件是由于动态创建需要加的
#include <asm/device.h> 
#include <linux/device.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL"); //!!!!

#define NUM_CMOS_BANKS 2
#define CMOS_BANK_SIZE (0xFF*8)
#define DEVICE_NAME "cmos"
#define CMOS_BANK0_INDEX_PORT 0x70

#define CMOS_BANK1_INDEX_PORT 0x71

#define CMOS_BANK0_DATA_PORT 0x72

#define CMOS_BANK1_DATA_PORT 0x73

unsigned char addrports[NUM_CMOS_BANKS] = {CMOS_BANK0_INDEX_PORT, CMOS_BANK1_INDEX_PORT};
unsigned char dataports[NUM_CMOS_BANKS] = {CMOS_BANK0_DATA_PORT, CMOS_BANK1_DATA_PORT};

/*Per-device (per-bank) strcture */
struct cmos_dev {
	unsigned short current_pointer;
	unsigned int size;
	int bank_number;
	struct cdev cdev;
	char name[10];
	
} *cmos_devp[NUM_CMOS_BANKS];

static struct file_operations cmos_fops = {
	.owner = THIS_MODULE,
	.open = cmos_open,
	
	
};

static dev_t cmos_dev_number;
struct class *cmos_class;

int __init
cmos_init(void) 
{
	int i,ret;
	
	if(alloc_chrdev_region(&cmos_dev_number, 0, NUM_CMOS_BANKS, DEVICE_NAME) < 0)
	{
		printk(KERN_DEBUG "can't register device\n");
		return -1;
	}
	
	cmos_class = class_create(THIS_MODULE, DEVICE_NAME);
	
	for(i = 0; i<NUM_CMOS_BANKS; i++)
	{
		cmos_devp[i] = kmalloc(sizeof(struct cmos_dev), GFP_KERNEL);
		if(!cmos_devp[i])
		{
			printk("bad malloc!\n");
			return -ENOMEM;
		}
		
		sprintf(cmos_devp[i]->name, "cmos%d", i);
		if(!(request_region(addrports[i], 2, cmos_devp[i]->name)))
		{
			printk("cmos I/O port 0%x is not free\n", addrports[i]);
			return -EIO;
		}
		
		cmos_devp[i]->bank_number = i;
		
		cdev_init(&cmos_devp[i]->cdev, &cmos_fops);
		cmos_devp[i]->cdev.owner = THIS_MODULE;
		
		ret = cdev_add(&cmos_devp[i]->cdev, (cmos_dev_number + i), 1);
		if(ret)
		{
			printk("Bad cdev\n");
		}
		device_create(cmos_class, NULL, MKDEV(MAJOR(cmos_dev_number), i), "cmos%d", (const char *)i);
	}
	
	printk("CMOS Driver Initialized.\n");			
	return 0;
}
	
	

void __exit
cmos_cleanup(void)
{
	int i;
	
	unregister_chrdev_region(MAJOR(cmos_dev_number), NUM_CMOS_BANKS);
	
	for(i = 0; i < NUM_CMOS_BANKS; i++)
	{
		device_destory(cmos_class, MKDEV(MAJOR(cmos_dev_number), i));
		release_region(addrports[i], 2);
		cdev_del(&cmos_devp[i]->cdev);
		kfree(cmos_devp[i]);		
	}
	
	class_destory(cmos_class);
	
	return;
}

int cmos_open()

module_init(cmos_init);
module_exit(cmos_cleanup);






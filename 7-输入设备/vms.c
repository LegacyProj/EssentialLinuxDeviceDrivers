#include<linux/init.h>  //初始换函数
#include<linux/kernel.h>  //内核头文件
#include<linux/module.h>  //模块的头文件

#include <linux/fs.h>
#include <linux/input.h>
#include <linux/platform_device.h>
//#include <linux/types.h>
//#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
//下面这三个头文件是由于动态创建需要加的
#include <asm/device.h> 
#include <linux/device.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL"); //!!!!


struct input_dev *vms_input_dev;
static struct platform_device *vms_dev;

//static ssize_t write_vms(struct )

static struct attribute *vms_attrs[] =
{
	&dev_attr_coordinates.attr,
	NULL
};


static struct attribute_group vms_attr_group = 
{
	.attrs = vms_attrs,
};

int __init vms_init(void)
{
	
	vms_dev = platform_device_register_simple("vms", -1, NULL, 0);
	if(IS_ERR(vms_dev))
	{
		printk("vms_init: error\n");
		return PTR_ERR(vms_dev);
	}
	
	sysfs_create_group(&vms_dev->dev.kobj, &vms_attr_group);
	
	vms_input_dev = input_allocate_device();
	if(!vms_input_dev)
	{
		printk("Bad input_allocate_device()\n");
		return -ENOMEM;
	}
	
	set_bit(EV_REL, vms_input_dev->evbit);
	set_bit(REL_X, vms_input_dev->relbit);
	set_bit(REL_Y, vms_input_dev->relbit);
	
	input_register_device(vms_input_dev);
	
	printk("virtual mouse driver initialized.\n");
	return 0;	
	
}

void __exit vms_cleanup(void)
{
	
}

module_init(vms_init);
module_exit(vms_cleanup);

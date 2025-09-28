#include "nxp_simtemp.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h> 

dev_t dev_simtemp = 0;
static struct cdev simtemp_cdev;
static struct class* simtemp_class;
static struct device* simtemp_device = NULL;

static int simtemp_open(struct inode* inode, struct file* file);
static int simtemp_release(struct inode* inode, struct file* file);
static ssize_t simtemp_read(struct file* file, char* buffer, size_t length, loff_t* offset);
static ssize_t simtemp_write(struct file* file, const char* buffer, size_t length, loff_t* offset);

static ssize_t simtemp_show(struct device* dev, struct device_attribute* att, char* buff);
static ssize_t simtemp_store(struct device* dev, struct device_attribute* att, const char* buff, size_t count);

/* Declare the sysfs entries. The macros create instances of dev_attr_**att_name** */
static DEVICE_ATTR(sampling_ms, 0644, simtemp_show, simtemp_store);
static DEVICE_ATTR(threshold_mC, 0644, simtemp_show, simtemp_store);
static DEVICE_ATTR(mode, 0644, simtemp_show, simtemp_store);
static DEVICE_ATTR(stats, 0444, simtemp_show, simtemp_store);

static struct attribute* a_attrs[] = {
  &dev_attr_sampling_ms.attr,
  &dev_attr_threshold_mC.attr,
  &dev_attr_mode.attr,
  &dev_attr_stats.attr,
  NULL,
};

static const struct attribute_group att_group =
{
  .attrs = a_attrs,
};

struct file_operations simtemp_fops = 
{
    .owner   = THIS_MODULE,
    .open    = simtemp_open,
    .release = simtemp_release,
    .read    = simtemp_read,
    .write   = simtemp_write,
};

static int __init simtemp_Init (void)
{
  if((alloc_chrdev_region(&dev_simtemp, 0, 1, "simtemp_dev")) <0)
  {
    pr_err("Cannot allocate major number\n");
    return -1;
  }
  
  pr_info("Major = %d Minor = %d \n",MAJOR(dev_simtemp), MINOR(dev_simtemp));
  cdev_init(&simtemp_cdev,&simtemp_fops);
  
  if((cdev_add(&simtemp_cdev,dev_simtemp,1)) < 0)
  {
    pr_err("Cannot add the device to the system\n");
    goto r_class;
  }
  
  if(IS_ERR(simtemp_class = class_create("simtemp_class")))
  {
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
  
  simtemp_device = device_create(simtemp_class,NULL,dev_simtemp,NULL,"simtemp_dev");
  if(simtemp_device == NULL)
  {
    pr_err("Cannot create the Device \n");
    goto r_device;
  }

  device_add_group(simtemp_device, &att_group);
  
  pr_info("Sim Temp Insert!!!\n");
  return 0;
  
  r_device:
    class_destroy(simtemp_class);
    return -1;
            
  r_class:
    unregister_chrdev_region(dev_simtemp,1);
    return -1;
}

static void __exit simtemp_Exit (void)
{
  device_remove_group(simtemp_device,&att_group);
  device_destroy(simtemp_class, dev_simtemp);
  class_destroy(simtemp_class);
  cdev_del(&simtemp_cdev);
  unregister_chrdev_region(dev_simtemp,1);
  pr_info("Sim Temp Removed!!!\n");
}

static int simtemp_open(struct inode* inode, struct file* file)
{
    pr_info("Open Function Called...!!!\n");
    return 0;
}

static int simtemp_release(struct inode* inode, struct file* file)
{
    pr_info("Release Function Called...!!!\n");
    return 0;
}

static ssize_t simtemp_read(struct file* file, char* buffer, size_t length, loff_t* offset)
{
   pr_info("Read Function Called...!!!\n");
    char string[5] = "Hola\n";
    
    // Simple version - always return the string
    if (copy_to_user(buffer, string, min(length, (size_t)5)) != 0)
        return -EFAULT;
    
    return 5;
}

static ssize_t simtemp_write(struct file* file, const char* buffer, size_t length, loff_t* offset)
{
    pr_info("Write Function Called...!!!\n");
    return 0;
}

static ssize_t simtemp_show(struct device* dev, struct device_attribute* att, char* buff)
{

  return 0;
}

static ssize_t simtemp_store(struct device* dev, struct device_attribute* att, const char* buff, size_t count)
{

  return 0;
}

module_init(simtemp_Init);
module_exit(simtemp_Exit);

MODULE_AUTHOR("Ismael Garcia Herrera");
MODULE_DESCRIPTION("Temperature Sensor Simulator");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

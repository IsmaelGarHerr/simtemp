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
#include <linux/string.h>

dev_t dev_simtemp = 0;
static struct cdev simtemp_cdev;
static struct class* simtemp_class;
static struct device* simtemp_device = NULL;

static int simtemp_open(struct inode* inode, struct file* file);
static int simtemp_release(struct inode* inode, struct file* file);
static ssize_t simtemp_read(struct file* file, char* buffer, size_t length, loff_t* offset);
static ssize_t simtemp_write(struct file* file, const char* buffer, size_t length, loff_t* offset);

static ssize_t simtemp_show(struct device* dev, struct device_attribute* dev_att, char* buff);
static ssize_t simtemp_store(struct device* dev, struct device_attribute* dev_att, const char* buff, size_t count);

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

static const struct attribute_group att_group = {
  .attrs = a_attrs,
};

static struct file_operations simtemp_fops = {
  .owner   = THIS_MODULE,
  .open    = simtemp_open,
  .release = simtemp_release,
  .read    = simtemp_read,
  .write   = simtemp_write,
};

static struct simtemp_cfg s_simtempConfig = {
  .sampling_rate_ms = 1,
  .temp_threshold   = 2,
  .sys_mode         = 3,
  .stats            = 4
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
  pr_info("Read Function Called %d::%d...!!!\n",length,offset);
  char string[5] = "Hola\n";
  
  if (copy_to_user(buffer, string, min(length, (size_t)5)) != 0)
  {
    return -EFAULT;
  }
    
  return 5;
}

static ssize_t simtemp_write(struct file* file, const char* buffer, size_t length, loff_t* offset)
{
  pr_info("Write Function Called...!!!\n");
  return 0;
}
  
static ssize_t simtemp_show(struct device* dev, struct device_attribute* dev_att, char* buff)
{
  u16 retVal = 0;
  if(strcmp(dev_att->attr.name,"sampling_ms") == 0)
  {
    retVal = snprintf(buff, PAGE_SIZE, "%d\n", s_simtempConfig.sampling_rate_ms);
  }
  else if(strcmp(dev_att->attr.name,"threshold_mC") == 0)
  {
    retVal = snprintf(buff, PAGE_SIZE, "%d\n", s_simtempConfig.temp_threshold);
  }
  else if(strcmp(dev_att->attr.name,"mode") == 0)
  {
    retVal = snprintf(buff, PAGE_SIZE, "%d\n", s_simtempConfig.sys_mode);
  }
  else if(strcmp(dev_att->attr.name,"stats") == 0)
  {
    retVal = snprintf(buff, PAGE_SIZE, "%d\n", s_simtempConfig.stats);
  }
  else
  {
    pr_info("Undefined attribute...!!!\n");
  }

  return retVal;
}

static ssize_t simtemp_store(struct device* dev, struct device_attribute* dev_att, const char* buff, size_t count)
{
  u16 retVal = count;
  s32 tempVal;
  const char* stringStart;
  s8 multiplier = 1;

  pr_info("Store function Called %s: %s, %d\n",dev_att->attr.name,buff,count);
  if(strcmp(dev_att->attr.name,"sampling_ms") == 0)
  {
    if(buff[0]=='-')
    {
      return -EINVAL;
    }
    retVal = kstrtouint(buff, 10, &tempVal);
    if(retVal == 0)
    {
      s_simtempConfig.sampling_rate_ms = (u16)tempVal;
    }
  }
  else if(strcmp(dev_att->attr.name,"threshold_mC") == 0)
  {
    if(buff[0]!='-'){
      stringStart = buff;
    }
    else{
      stringStart = &buff[1];
      multiplier  = -1;
    }

    retVal = kstrtouint(stringStart, 10, &tempVal);
    if (retVal == 0) {
      s_simtempConfig.temp_threshold = tempVal*multiplier;
    }
  }
  else if(strcmp(dev_att->attr.name,"mode")== 0)
  {
    if(buff[0]=='-')
    {
      return -EINVAL;
    }
    retVal = kstrtouint(buff, 10, &tempVal);
    if((retVal == 0) && (tempVal < SYS_MAX_MODE))
    {
      s_simtempConfig.sys_mode = (u8)tempVal;
    }
  }
  else
  {
    pr_info("Undefined attribute...!!!\n");
  }

  if (retVal < 0) {
    pr_err("Error %d parsing value for attribute %s\n",retVal, dev_att->attr.name);
    return retVal;
  }

  return count;
}

module_init(simtemp_Init);
module_exit(simtemp_Exit);

MODULE_AUTHOR("Ismael Garcia Herrera");
MODULE_DESCRIPTION("Temperature Sensor Simulator");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

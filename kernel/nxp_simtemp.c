#include "nxp_simtemp.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init lkm_Init (void)
{
    printk(KERN_INFO "tempsim Test Module init \n");

    return 0;
}

static void __exit lkm_Exit (void)
{
    printk(KERN_INFO "tempsim Test Module exit \n");
}

module_init(lkm_Init);
module_exit(lkm_Exit);

MODULE_AUTHOR("Ismael Garcia Herrera");
MODULE_DESCRIPTION("Temperature Sensor Simulator");
MODULE_LICENSE("MIT");
MODULE_VERSION("0.1");

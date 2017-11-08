#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/uaccess.h>

#define BULK_EP_OUT 0x04   //address where arduino device allow write
#define BULK_EP_IN 0x83		//address where arduino device allow read
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define MAX_PKT_SIZE 512

#define DRIVER_AUTHOR "Braisman"
#define DRIVER_DESC "Serial write to an Arduino"

#define VENDOR_ID 0x2341   // vendor_id for arduino
#define PRODUCT_ID 0x0043	// product_id for arduino


static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void arduino_disconnect(struct usb_interface *interface);

/*****************************
Driver description
******************************/

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("0.7.1");

/*****************************
Device actions
******************************/

//implements open by the OS
static int arduino_open(struct inode *i, struct file *f)
{
    printk("Open()\n");
    return 0;
}

//implements close by the OS
static int arduino_close(struct inode *i, struct file *f)
{
    printk("Close()\n");
    return 0;
}

//handle an write function for a driver file,
//and send the second character to device, use 9600 bauds like typically in arduino
static ssize_t arduino_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
    int retval;
	char value[2];
    int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);

    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
    {
        return -EFAULT;
    }
 	value[1] = bulk_buf[0];
    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
            value, 2, &wrote_cnt, 9600);
    printk("Bulk message sent in %d, %s, size %zu, returned %d\n", BULK_EP_OUT, bulk_buf, cnt, wrote_cnt);
    if (retval)
    {
        printk("Bulk message returned %d\n", retval);
        return retval;
    }

    return wrote_cnt;
}

//mappping operations to main functions in the driver
static struct file_operations fops =
{
    .open = arduino_open,
    .release = arduino_close,
    .write = arduino_write,
};

/*****************************
Device initialization
******************************/

//Routine for probe the correct operations of driver
static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;

    device = interface_to_usbdev(interface);

    class.name = "usb/arduino%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
        /* Something prevented us from registering this driver */
        printk("Not able to get a minor for this device\n");
    }
    else
    {
        printk("Minor obtained: %d\n", interface->minor);
    }

    return retval;
}

//handle the disconnect of device
static void arduino_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &class);
}

/* Table of devices that work with this driver */
static struct usb_device_id arduino_table[] =
{
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, arduino_table);

//describe the characteristics of the module
static struct usb_driver arduino_driver =
{
    .name = "arduino_driver",
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
    .id_table = arduino_table,
};

/*****************************
Driver initialization
******************************/

//initialize driver, run on insmod instruction on user space
static int __init arduino_init(void)
{
    int result;

    /* Register this driver with the USB subsystem */
    if ((result = usb_register(&arduino_driver)))
    {
	printk("usb_register failed. Error number %d\n", result);
    }
    return result;
}

//finish drive, run on rmmod instruction on user space
static void __exit arduino_exit(void)
{
    /* Deregister this driver with the USB subsystem */
    usb_deregister(&arduino_driver);
}

module_init(arduino_init);
module_exit(arduino_exit);

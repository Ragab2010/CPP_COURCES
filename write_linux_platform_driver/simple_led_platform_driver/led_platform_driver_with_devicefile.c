#include <linux/module.h>// for module macros
#include <linux/platform_device.h>// for platform driver
#include <linux/gpio/consumer.h>// for gpiod functions
#include <linux/fs.h>// for file operations
#include <linux/cdev.h>// for character device
#include <linux/device.h>// for class_create, device_create
#include <linux/uaccess.h>// for copy_to_user, copy_from_user
#include <linux/mutex.h>// for mutex
#include <linux/of.h>// for of_match_ptr
#include <linux/slab.h>// for devm_kzalloc

#define DRIVER_NAME  "led_gpio17"// name of the driver
#define DEVICE_NAME  "led_gpio17"// name of the device file

struct led_gpio_dev {
    struct gpio_desc *led_gpiod;// the GPIO descriptor for the LED GPIO
    struct mutex lock;//   a mutex to protect access to the device state
    int value;// current value of the LED (0 = off, 1 = on)

    dev_t devt;// device number for the character device
    struct cdev cdev;// character device structure
    struct class *class;//  device class for creating the device node
    struct device *device;// device structure for the created device
};

/* ========== Character Device Operations ========== */
static ssize_t led_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct led_gpio_dev *led = file->private_data;
    char val;
    int ret;

    mutex_lock(&led->lock);
    val = led->value ? '1' : '0';
    mutex_unlock(&led->lock);

    if (*ppos != 0)
        return 0;

    ret = copy_to_user(buf, &val, 1);
    if (ret)
        return -EFAULT;

    *ppos = 1;
    return 1;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct led_gpio_dev *led = file->private_data;
    char val;
    int ret;

    if (count < 1)
        return -EINVAL;

    ret = copy_from_user(&val, buf, 1);
    if (ret)
        return -EFAULT;

    mutex_lock(&led->lock);
    if (val == '1') {
        led->value = 1;
        gpiod_set_value(led->led_gpiod, 1);
    } else {
        led->value = 0;
        gpiod_set_value(led->led_gpiod, 0);
    }
    mutex_unlock(&led->lock);

    return count;
}

static int led_open(struct inode *inode, struct file *file)
{
    /*
    here we use container_of to get the led_gpio_dev structure from the inode's cdev member.
    then we store the pointer to the led_gpio_dev structure in the file's private_data member
    for use in other file operations like read and write.
    --equal to file->private_data = inode->i_cdev->led_gpio_dev
    -this data that we store it at led_driver_probe function when we initialize the cdev member of led_gpio_dev structure.
    */
    struct led_gpio_dev *led = container_of(inode->i_cdev, struct led_gpio_dev, cdev);
    file->private_data = led;
    return 0;
}

static const struct file_operations led_fops = {
    .owner  = THIS_MODULE,
    .read   = led_read,
    .write  = led_write,
    .open   = led_open,
};


/* ========== Probe / Remove ========== */
static int led_driver_probe(struct platform_device *pdev)
{
    struct led_gpio_dev *led;
    int ret;

    dev_info(&pdev->dev, "Probing LED GPIO driver\n");

    /*
    here we allocate memory for our led_gpio_dev structure using devm_kzalloc.
    we pass the device structure of the platform device (pdev->dev), the size of our
    led_gpio_dev structure, and GFP_KERNEL as the allocation flag.
    if allocation fails, we return -ENOMEM.
    */
    led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
    if (!led)
        return -ENOMEM;

    mutex_init(&led->lock);

    /*
    here we use devm_gpiod_get to acquire the GPIO descriptor for the LED GPIO.
    we pass the device structure of the platform device (pdev->dev), the GPIO name "led"
    which should match the name in the device tree, and the flag GPIOD_OUT_LOW to configure
    the GPIO as an output initialized to low (LED off).
    if acquiring the GPIO fails, we log an error message and return the error code.
    */
    led->led_gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led->led_gpiod)) {
        dev_err(&pdev->dev, "Failed to get LED GPIO\n");
        return PTR_ERR(led->led_gpiod);
    }

    /* Allocate character device number */
    /*
    here we use alloc_chrdev_region to dynamically allocate a major and minor number for our character device.
    we store the allocated device number in led->devt, request 1 minor number, and use DEVICE_NAME as the name for the device.
    if allocation fails, we log an error message and return the error code.
    */
    ret = alloc_chrdev_region(&led->devt, 0, 1, DEVICE_NAME);
    if (ret) {
        dev_err(&pdev->dev, "Failed to allocate chrdev region\n");
        return ret;
    }

    /* Create class */
    /*
    here we use class_create to create a device class for our LED device.
    we pass THIS_MODULE as the owner and DEVICE_NAME as the name of the class.
    if class creation fails, we clean up by unregistering the character device number
    and return the error code.
    */
    led->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(led->class)) {
        ret = PTR_ERR(led->class);
        goto err_unregister_chrdev;
    }

    /* Create device under /dev */
    /*
    here we use device_create to create a device node under /dev with the name DEVICE_NAME.
    we pass the class created earlier, the parent device (pdev->dev), the device number (led->devt),
    and the led structure as the driver data for the device.
    this allows us to access the led structure in other parts of the driver, such as in sysfs attribute handlers ,open,read,write functions.
    we retreive it using dev_get_drvdata when needed.
    equal to led->device->driver_data = led
     */
    led->device = device_create(led->class, &pdev->dev, led->devt, led, DEVICE_NAME);
    if (IS_ERR(led->device)) {
        ret = PTR_ERR(led->device);
        goto err_destroy_class;
    }

    /*
    here we initialize the cdev structure within our led_gpio_dev structure.
    we use cdev_init to set up the cdev with our file operations (led_fops) and set the owner to THIS_MODULE.
    then we add the cdev to the system using cdev_add, associating it with the allocated device number (led->devt) and
    specifying that we are using 1 minor number.
    if adding the cdev fails, we clean up by destroying the device and class, unregistering the character device number, and return the error code.
    */
    cdev_init(&led->cdev, &led_fops);
    led->cdev.owner = THIS_MODULE;

    /*
    here we add the cdev to the system using cdev_add, associating it with the allocated device number (led->devt) and
    specifying that we are using 1 minor number.
    if adding the cdev fails, we clean up by destroying the device and class, unregistering the character device number, and return the error code.
    -equal to ret = cdev_add(&led->cdev, led->devt, 1);
    -also this same as ret = cdev_add(inode->i_cdev, led->devt, 1);
    -we pass &led->cdev as the first argument to cdev_add which is the cdev member of led_gpio_dev structure.
    */
    ret = cdev_add(&led->cdev, led->devt, 1);
    if (ret) {
        dev_err(&pdev->dev, "Failed to add cdev\n");
        goto err_destroy_device;
    }

    /* Attach driver data to device for sysfs */
    /*
    here we use dev_set_drvdata to associate the led structure with the device created under /dev.
    This allows us to access the led structure in other parts of the driver, such as in sysfs attribute handlers ,open,read,write functions.
    we retreive it using dev_get_drvdata when needed.
    equal to led->device->driver_data = led
    also this same as led->device = device_create(led->class, &pdev->dev, led->devt, led, DEVICE_NAME);
    we pass led as the last argument to device_create which sets the driver_data of the created device to led.
    */
    dev_set_drvdata(led->device, led);

    /*
    we use platform_set_drvdata to associate the led structure with the platform device.
    This allows us to access the led structure in other parts of the driver, such as in the remove function.
    we retreive it using platform_get_drvdata when needed.
    equal to pdev->dev.driver_data = led
    also this same as platform_set_drvdata(pdev, led);
    we pass led as the second argument to platform_set_drvdata which sets the driver_data of the platform device to led.
    */
    platform_set_drvdata(pdev, led);
    dev_info(&pdev->dev, "LED driver initialized successfully\n");
    return 0;

err_cdev_del:
    cdev_del(&led->cdev);
err_destroy_device:
    device_destroy(led->class, led->devt);
err_destroy_class:
    class_destroy(led->class);
err_unregister_chrdev:
    unregister_chrdev_region(led->devt, 1);
    return ret;
}

static int led_driver_remove(struct platform_device *pdev)
{
    /*
    here we use platform_get_drvdata to retrieve the led structure associated with the platform device.
    we then use this structure to clean up resources allocated during probe, such as removing sysfs attributes,
    deleting the character device, destroying the device and class, and unregistering the character device number.  
    equal to struct led_gpio_dev *led = pdev->dev.driver_data
    also this same as struct led_gpio_dev *led = dev_get_drvdata(&pdev->dev);
    we pass &pdev->dev as the argument to dev_get_drvdata which retrieves the driver_data of the device associated with the platform device.
    */
    struct led_gpio_dev *led = platform_get_drvdata(pdev);

    gpiod_set_value(led->led_gpiod, 0);
    //sysfs_remove_group(&led->device->kobj, &led_attr_group);
    cdev_del(&led->cdev);
    device_destroy(led->class, led->devt);
    class_destroy(led->class);
    unregister_chrdev_region(led->devt, 1);

    dev_info(&pdev->dev, "LED driver removed\n");
    return 0;
}

/* ========== Device Tree Match ========== */
/*
    here we define the device tree match table for our LED GPIO driver.
    we specify the compatible string "rpi,led-gpio17" which should match the compatible property
    in the device tree node for our LED GPIO device.
    this allows the driver to be matched with devices described in the device tree.
*/
static const struct of_device_id led_of_match[] = {
    { .compatible = "rpi,led-gpio17" },
    { }
};
/*
    we use MODULE_DEVICE_TABLE to export the device tree match table
    so that user-space tools can access it.
*/
MODULE_DEVICE_TABLE(of, led_of_match);

/* ========== Platform Driver ========== */
/*
    here we define the platform driver structure for our LED GPIO driver.
    we set the probe and remove functions to led_driver_probe and led_driver_remove respectively.
    we also set the driver name to DRIVER_NAME and provide the device tree match table (led_of_match)
    to allow the driver to be matched with devices described in the device tree.
*/
static struct platform_driver led_gpio_driver = {
    .probe  = led_driver_probe,
    .remove = led_driver_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = led_of_match,
    },
};
/*
    * Use module_platform_driver to create the module init and exit functions
    * which register and unregister the platform driver.
*/
module_platform_driver(led_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ragab Elsayed");
MODULE_DESCRIPTION("Platform driver for LED GPIO with /dev and sysfs support");
MODULE_VERSION("2.0");

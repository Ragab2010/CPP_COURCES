#include <linux/module.h>// for module macros
#include <linux/platform_device.h>// for platform driver
#include <linux/gpio/consumer.h>// for gpiod functions
#include <linux/sysfs.h>// for sysfs functions
#include <linux/slab.h>// for devm_kzalloc
#include <linux/mutex.h>// for mutex
#include <linux/of.h>// for of_match_ptr
#include <linux/kdev_t.h>// for MKDEV
#include <linux/cdev.h>// for cdev
#include <linux/device.h>// for class_create, device_create
#include <linux/uaccess.h>// for copy_to_user, copy_from_user

#define DRIVER_NAME "led_gpio17"// name of the driver
#define DEVICE_NAME "led_gpio17"// name of the device file

struct led_gpio_dev {
    struct gpio_desc *led_gpiod;// the GPIO descriptor for the LED GPIO
    struct mutex lock;//   a mutex to protect access to the device state
    int value;//    current value of the LED (0 = off, 1 = on)
    struct class *led_class;// device class for creating the device node
    struct device *dev;// device structure for the created device
};

/* Sysfs show/store */
/*
here we define the sysfs show function for the "value" attribute.
*/
static ssize_t value_show(struct device *dev,
                          struct device_attribute *attr, char *buf)
{
    /*
    here we retrieve our led_gpio_dev structure associated with the device
    using dev_get_drvdata.
    */
    struct led_gpio_dev *led = dev_get_drvdata(dev);
    int val;

    mutex_lock(&led->lock);
    /*
    here we read the current LED value from our device structure.
    */
    val = led->value;
    mutex_unlock(&led->lock);

    return sysfs_emit(buf, "%d\n", val);
}
/*
here we define the sysfs store function for the "value" attribute.
*/
static ssize_t value_store(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf, size_t count)
{
    /*
    here we retrieve our led_gpio_dev structure associated with the device
    using dev_get_drvdata.
    */
    struct led_gpio_dev *led = dev_get_drvdata(dev);
    long val;

    /*
    here we convert the input string from sysfs to a long integer using kstrtol.
    we specify base 10 for decimal conversion.
    if conversion fails, we return -EINVAL to indicate invalid input.
    */
    if (kstrtol(buf, 10, &val))
        return -EINVAL;

    mutex_lock(&led->lock);
    /*
    here we normalize the input value to either 0 or 1 using !!val.
    */
    led->value = !!val; /* normalize to 0 or 1 */
    /*
    here we set the GPIO value to turn the LED on or off using gpiod_set_value.
    */
    gpiod_set_value(led->led_gpiod, led->value);
    mutex_unlock(&led->lock);

    dev_info(dev, "LED set to %ld\n", val);
    return count;
}

/*
here we define the device attribute for the "value" attribute,
making it readable and writable.
DEVICE_ATTR_RW(value) is a macro that creates a struct device_attribute
with the appropriate show and store functions.
DEVICE_ATTR_RW(value) is macro equal to __ATTR_RW(value) __ATTR(_name, 0644, value_show, value_store)   
DEVICE_ATTR_RW(value) equal to  
static struct device_attribute dev_attr_value = {
    .attr = {
        .name = "value",
        .mode = 0644,
    },
    .show = value_show,
    .store = value_store,
};
*/
static DEVICE_ATTR_RW(value);


/*
here we define an array of attributes for the LED device.
we include the "value" attribute and terminate the array with NULL.

*/
static struct attribute *led_attrs[] = {
    &dev_attr_value.attr,
    NULL,
};
/*
here we define an attribute group for the LED device.
we set the attrs member to the led_attrs array defined above.
*/
static const struct attribute_group led_attr_group = {
    .attrs = led_attrs,
};

/* Probe */
static int led_driver_probe(struct platform_device *pdev)
{
    int ret;// return value for error handling
    struct led_gpio_dev *led;// pointer to our device structure

    dev_info(&pdev->dev, "Probing LED GPIO driver\n");

    /* Allocate device struct */
    /*
    here we allocate memory for our led_gpio_dev structure using devm_kzalloc.
    we pass the device structure of the platform device (pdev->dev), the size of our
    led_gpio_dev structure, and GFP_KERNEL as the allocation flag.
    if allocation fails, we return -ENOMEM.
    */
    led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
    if (!led)
        return -ENOMEM;

    /*
    here we initialize the mutex in our led_gpio_dev structure to protect access to the device state.
    */
    mutex_init(&led->lock);

    /* Get GPIO descriptor from device tree */
    /*
    here we use devm_gpiod_get to acquire the GPIO descriptor for the LED GPIO.
    we pass the device structure of the platform device (pdev->dev), the GPIO name "led",
    and GPIOD_OUT_LOW to configure the GPIO as an output initialized to low (LED off).
    if acquiring the GPIO descriptor fails, we log an error message and return the error code.
    */
    led->led_gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led->led_gpiod)) {
        dev_err(&pdev->dev, "Failed to get LED GPIO\n");
        return PTR_ERR(led->led_gpiod);
    }

    led->value = 0;// initialize LED state to off
    
    
    /*
    here we associate our led_gpio_dev structure with the platform device
    so that we can retrieve it later in other driver functions as remove and sysfs callbacks.  
    equal to pdev->dev.driver_data = led;
    */
    platform_set_drvdata(pdev, led);

    /* Create class */
    /*
    here we create a device class for our LED device using class_create.
    we pass THIS_MODULE as the owner and DEVICE_NAME as the class name.
    if class creation fails, we retrieve the error code using PTR_ERR, log an error message,
    and return the error code.
    */
    led->led_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(led->led_class)) {
        ret = PTR_ERR(led->led_class);
        dev_err(&pdev->dev, "Failed to create class\n");
        return ret;
    }

    /* Create device under the class */
    /*
    here we create a device node under /dev using device_create.
    we pass the class created earlier (led->led_class), NULL for the parent device,
    MKDEV(0, 0) for the device number (we will use dynamic allocation   later),
    led as the driver data for the device, and DEVICE_NAME as the device name.
    if device creation fails, we retrieve the error code using PTR_ERR,
    clean up by destroying the class, log an error message, and return the error code.
    -equal to led->dev = device_create(led->led_class, NULL, MKDEV(0, 0), led, DEVICE_NAME); 
    -also equal to led->dev = device_create(led->led_class, &pdev->dev, led->devt, led, DEVICE_NAME); 
    -we pass led as the fourth argument to device_create which sets the driver_data member of the created device structure.
    -this allows us to access the led structure in other parts of the driver, such as in sysfs attribute handlers.
    -we retreive it using dev_get_drvdata when needed.
    */
    led->dev = device_create(led->led_class, NULL, MKDEV(0, 0), led, DEVICE_NAME);
    if (IS_ERR(led->dev)) {
        ret = PTR_ERR(led->dev);
        class_destroy(led->led_class);
        dev_err(&pdev->dev, "Failed to create device\n");
        return ret;
    }

    /* Set driver data for sysfs callbacks */
    /*
    here we use dev_set_drvdata to associate the led structure with the device created under /dev.
    This allows us to access the led structure in other parts of the driver, such as in sysfs attribute handlers ,open,read,write functions.
    we retreive it using dev_get_drvdata when needed.
    equal to led->device->driver_data = led
    also this same as led->dev = device_create(led->led_class, NULL, MKDEV(0, 0), led, DEVICE_NAME);
    we pass led as the last argument to device_create which sets the driver_data of the created device to led.
    */
    dev_set_drvdata(led->dev, led);

    /* Create sysfs group */
    /*
    here we create a sysfs attribute group for our device using sysfs_create_group.
    we pass the kobject of the device (led->device->kobj) and a pointer to our attribute group (led_attr_group).
    if creating the sysfs group fails, we clean up by deleting the cdev, destroying the device and class,
    unregistering the character device number, log an error message, and return the error code.
    equal to ret = sysfs_create_group(&led->device->kobj, &led_attr_group); 
    -we pass &led->device->kobj as the first argument to sysfs_create_group which is the kobject member of the device structure.
    -we pass &led_attr_group as the second argument which is the attribute group we defined earlier containing our sysfs attributes.
    -this creates the sysfs files under /sys/class/led_gpio17/led_gpio17/value  for our LED device.
    */
    ret = sysfs_create_group(&led->dev->kobj, &led_attr_group);
    if (ret) {
        device_destroy(led->led_class, MKDEV(0, 0));
        class_destroy(led->led_class);
        return ret;
    }

    dev_info(&pdev->dev, "LED driver initialized successfully\n");
    return 0;
}

/* Remove */
static int led_driver_remove(struct platform_device *pdev)
{
    /*
    here we retrieve our led_gpio_dev structure associated with the platform device.
    if the structure is NULL, we return 0 as there is nothing to clean up.
    then we turn off the LED by setting its GPIO value to 0.
    we remove the sysfs attribute group, destroy the device and class created earlier,
    and log a message indicating that the driver has been removed.
    */
    struct led_gpio_dev *led = platform_get_drvdata(pdev);

    if (!led)
        return 0;

    gpiod_set_value(led->led_gpiod, 0);

    /* Remove sysfs and device class */
    if (led->dev)
        sysfs_remove_group(&led->dev->kobj, &led_attr_group);

    if (led->led_class && led->dev)
        device_destroy(led->led_class, MKDEV(0, 0));

    if (led->led_class)
        class_destroy(led->led_class);

    dev_info(&pdev->dev, "LED driver removed\n");
    return 0;
}

/* Device Tree match */
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

/* Platform driver struct */
/*
here we define the platform driver structure for our LED GPIO driver.
we set the probe and remove functions to led_driver_probe and led_driver_remove respectively.
we also set the driver name to DRIVER_NAME and provide the device tree match table (led_of_match)
to allow the driver to be matched with devices described in the device tree.
*/
static struct platform_driver led_gpio_driver = {
    .probe = led_driver_probe,
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
MODULE_DESCRIPTION("Platform driver for LED GPIO with sysfs attribute and device class");
MODULE_VERSION("1.0");

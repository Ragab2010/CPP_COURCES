#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>  // For gpio_to_desc
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "led_gpio17"
#define DEVICE_NAME "led_gpio17"

/* Platform data structure */
struct gpio17_platform_data {
    const char *gpio_name;
    int gpio_pin;
    int default_state;
};

struct led_gpio_dev {
    struct gpio_desc *led_gpiod;
    struct mutex lock;
    int value;
    struct class *led_class;
    struct device *dev;
};

/* Sysfs show/store */
static ssize_t value_show(struct device *dev,
                          struct device_attribute *attr, char *buf)
{
    struct led_gpio_dev *led = dev_get_drvdata(dev);
    int val;

    mutex_lock(&led->lock);
    val = led->value;
    mutex_unlock(&led->lock);

    return sysfs_emit(buf, "%d\n", val);
}

static ssize_t value_store(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf, size_t count)
{
    struct led_gpio_dev *led = dev_get_drvdata(dev);
    long val;

    if (kstrtol(buf, 10, &val))
        return -EINVAL;

    mutex_lock(&led->lock);
    led->value = !!val;
    gpiod_set_value(led->led_gpiod, led->value);
    mutex_unlock(&led->lock);

    dev_info(dev, "LED set to %ld\n", val);
    return count;
}

static DEVICE_ATTR_RW(value);

static struct attribute *led_attrs[] = {
    &dev_attr_value.attr,
    NULL,
};

static const struct attribute_group led_attr_group = {
    .attrs = led_attrs,
};

/* Probe function - Updated to correctly handle manual GPIO */
static int led_driver_probe(struct platform_device *pdev)
{
    int ret;
    struct led_gpio_dev *led;
    struct gpio17_platform_data *pdata = dev_get_platdata(&pdev->dev);

    dev_info(&pdev->dev, "Probing LED GPIO driver\n");

    /* Check if we have platform data (manual registration) */
    if (pdata) {
        dev_info(&pdev->dev, "Using platform data: GPIO%d, name: %s\n",
                 pdata->gpio_pin, pdata->gpio_name);
    } else {
        dev_info(&pdev->dev, "No platform data, using Device Tree\n");
    }

    /* Allocate device struct */
    led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
    if (!led)
        return -ENOMEM;

    mutex_init(&led->lock);

    /* Get GPIO descriptor */
    if (pdata) {
        /* Manual registration: convert GPIO pin number to descriptor */
        led->led_gpiod = gpio_to_desc(pdata->gpio_pin);
        if (!led->led_gpiod) {
            dev_err(&pdev->dev, "Failed to get GPIO descriptor for pin %d\n", pdata->gpio_pin);
            return -EINVAL;
        }

        /* Request the GPIO explicitly to ensure it's reserved */
        ret = gpiod_direction_output(led->led_gpiod, pdata->default_state);
        if (ret) {
            dev_err(&pdev->dev, "Failed to set GPIO%d as output: %d\n", pdata->gpio_pin, ret);
            return ret;
        }

        /* Set GPIO label if provided */
        if (pdata->gpio_name)
            gpiod_set_consumer_name(led->led_gpiod, pdata->gpio_name);
    } else {
        /* Device Tree approach */
        led->led_gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
        if (IS_ERR(led->led_gpiod)) {
            dev_err(&pdev->dev, "Failed to get LED GPIO from DT: %ld\n", PTR_ERR(led->led_gpiod));
            return PTR_ERR(led->led_gpiod);
        }
    }

    led->value = 0;
    platform_set_drvdata(pdev, led);

    /* Create class */
    led->led_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(led->led_class)) {
        ret = PTR_ERR(led->led_class);
        dev_err(&pdev->dev, "Failed to create class\n");
        return ret;
    }

    /* Create device under the class */
    led->dev = device_create(led->led_class, NULL, MKDEV(0, 0), led, DEVICE_NAME);
    if (IS_ERR(led->dev)) {
        ret = PTR_ERR(led->dev);
        class_destroy(led->led_class);
        dev_err(&pdev->dev, "Failed to create device\n");
        return ret;
    }

    /* Set driver data for sysfs callbacks */
    dev_set_drvdata(led->dev, led);

    /* Create sysfs group */
    ret = sysfs_create_group(&led->dev->kobj, &led_attr_group);
    if (ret) {
        device_destroy(led->led_class, MKDEV(0, 0));
        class_destroy(led->led_class);
        return ret;
    }

    dev_info(&pdev->dev, "LED driver initialized successfully\n");
    return 0;
}

/* Remove function */
static int led_driver_remove(struct platform_device *pdev)
{
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

/* Platform driver struct */
static struct platform_driver led_gpio_driver = {
    .probe = led_driver_probe,
    .remove = led_driver_remove,
    .driver = {
        .name = DRIVER_NAME,
    },
};

module_platform_driver(led_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ragab Elsayed");
MODULE_DESCRIPTION("Platform driver for LED GPIO17 with manual device support");
MODULE_VERSION("1.0");
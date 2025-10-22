#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/slab.h>

#define DRIVER_NAME "led_gpio17"
#define GPIO_LED_PIN 17

/* Platform device data */
struct gpio17_platform_data {
    const char *gpio_name;
    int gpio_pin;
    int default_state;
};

static struct gpio17_platform_data gpio17_data = {
    .gpio_name = "led",
    .gpio_pin = GPIO_LED_PIN,
    .default_state = 0,
};

/* Device release function */
static void manual_led_device_release(struct device *dev)
{
    pr_info("manual_led_device: device released\n");
}

/* Platform device structure */
static struct platform_device manual_led_device = {
    .name = DRIVER_NAME,  // Must match the driver's .name
    .id = -1,
    .dev = {
        .platform_data = &gpio17_data,
        .release = manual_led_device_release,
    },
};

/* Module initialization */
static int __init manual_led_device_init(void)
{
    int ret;

    pr_info("Manual LED Device Module: Initializing...\n");

    /* Register the platform device */
    ret = platform_device_register(&manual_led_device);
    if (ret) {
        pr_err("Failed to register platform device: %d\n", ret);
        return ret;
    }

    pr_info("Manual LED device registered successfully\n");
    pr_info("Device name: %s\n", manual_led_device.name);
    pr_info("GPIO pin: %d\n", GPIO_LED_PIN);
    pr_info("Ready for driver '%s' to bind\n", DRIVER_NAME);

    return 0;
}

/* Module cleanup */
static void __exit manual_led_device_exit(void)
{
    pr_info("Manual LED Device Module: Unloading...\n");
    platform_device_unregister(&manual_led_device);
    pr_info("Manual LED device unregistered\n");
}

module_init(manual_led_device_init);
module_exit(manual_led_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ragab Elsayed");
MODULE_DESCRIPTION("Manual platform device registration for LED GPIO17");
MODULE_VERSION("1.0");
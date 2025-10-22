#include <linux/module.h>
#include <linux/platform_device.h>

static struct gpio17_platform_data led_pdata = {
    .gpio_name = "led-gpio17",
    .gpio_pin = 17, /* Adjust this based on your hardware */
    .default_state = 0,
};

static struct platform_device led_pdev = {
    .name = DRIVER_NAME,
    .id = -1,
    .dev = {
        .platform_data = &led_pdata,
    },
};

static int __init led_device_init(void)
{
    return platform_device_register(&led_pdev);
}

static void __exit led_device_exit(void)
{
    platform_device_unregister(&led_pdev);
}

module_init(led_device_init);
module_exit(led_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ragab Elsayed");
MODULE_DESCRIPTION("Platform device for LED GPIO17");
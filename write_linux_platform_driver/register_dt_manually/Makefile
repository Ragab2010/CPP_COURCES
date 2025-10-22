obj-m += manual_gpio_device.o
obj-m += gpio_led_driver.o
# obj-m += manual_gpio_device2

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

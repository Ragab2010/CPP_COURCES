obj-m += simple_led_platform_driver_attributes_file.o simple_led_platform_driver_attributes_group.o led_platform_driver_with_sysfs_attributes.o led_platform_driver_with_devicefile.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


# Overlay source and output
DTS_SRC = gpio17-ragab1.dts
DTBO_OUT = gpio17-ragab.dtbo


# Compile Device Tree Overlay
dtbo:
	dtc -@ -I dts -O dtb -o $(DTBO_OUT) $(DTS_SRC)

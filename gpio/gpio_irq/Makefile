obj-m := gpio_irq.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR=/home/k0v4/coding/linux/source/bbb_linux_5.4.106/
HOST_KERN_DIR=/lib/modules/$(shell uname -r)/build/
DTB_FILE_NAME=am335x-boneblack.dtb

all:
		make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules

clean:
		make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean

help:
		make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help

host:
		make -C $(HOST_KERN_DIR) M=$(PWD) modules

copy-dtb:
		scp $(KERN_DIR)arch/arm/boot/dts/$(DTB_FILE_NAME) debian@192.168.1.184:/home/debian/drivers

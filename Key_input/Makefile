
MODULE_NAME  := Gpio_Key
ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /home/zhaohe/HISI/Work/Hi3518_SDK_V1.0.B.0/osdrv/kernel/linux-3.0.y
# KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD)
# arm-none-linux-gnueabi-gcc misc_test.c  -o misc_test.out
	cp  *.ko /nfs/fs_rootfs

modules_install:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.out  *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules*


install:
	sudo insmod ./$(MODULE_NAME).ko
remove:
	sudo rmmod $(MODULE_NAME)

.PHONY: modules modules_install clean install remove

else
    obj-m  := $(MODULE_NAME).o
endif

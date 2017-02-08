
MODULE_NAME  := ms41908_hisi_spi
MODULE_TEST_NAME := $(MODULE_NAME)_test
ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /home/zhaohe/HISI/Work/Hi3518_SDK_V1.0.B.0/osdrv/kernel/linux-3.0.y
# KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD)
	arm-hisiv100nptl-linux-gcc -o $(MODULE_TEST_NAME).out  $(MODULE_TEST_NAME).c
	cp  *.ko /nfs/fs_rootfs
	cp  *.out /nfs/fs_rootfs

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

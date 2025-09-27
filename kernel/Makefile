obj-m += nxp_simtemp.o

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR ?= $(PWD)/build
BUILD_DIR_MAKEFILE ?= $(PWD)/build/Makefile

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR)  M=$(shell pwd) clean
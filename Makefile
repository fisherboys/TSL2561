#
#Makefile for tsl2561 driver.
#cmd:
#	make:
#       function: make a module
#   make clean:
#		function: clean all built files
#

obj-m := tsl2561.o

KDIR := [your kernel directory]
all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.bak *.order


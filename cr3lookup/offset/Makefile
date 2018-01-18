obj-m += offset.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules -lcsys

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	dmesg -c
	insmod offset.ko
	dmesg

uninstall:
	rmmod offset
	dmesg -c


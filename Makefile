obj-m+=man.o
kern = $(shell uname -r)

all:
	make -C /lib/modules/$(kern)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(kern)/build M=$(PWD) clean


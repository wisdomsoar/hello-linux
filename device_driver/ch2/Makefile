#
# Makefile by appleboy <appleboy.tw at="" gmail.com="">
#
obj-m       += hello.o
KVERSION := $(shell uname -r)

all:
#shell command: make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
#-C: change working directory to find makefile (?)
#M: come back hello folder and build module target
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
#^^^
##Makefile must use <tab> to do indent

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) 


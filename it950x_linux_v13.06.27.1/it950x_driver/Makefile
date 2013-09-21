module=usb-it950x
EXTRA_CFLAGS = -DEXPORT_SYMTAB
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
MACHINE = $(shell uname -m)
KDIR26	 := /lib/modules/$(CURRENT)/kernel/drivers/media
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)
LMDIR26 := /lib/firmware

usb-it950x-objs := \
		it950x-core.o it950x-drv.o iocontrol.o  \
		tuner.o cmd.o IT9133.o ADF4351.o \
		IT9507.o Omega.o eagleTuner.o \
		standard.o usb2impl.o modulatorUser.o user.o 

obj-m += usb-it950x.o 

default:
	@(cp api/*.* ./; cp src/*.* ./)
	make -s -C $(KDIR) SUBDIRS=$(PWD) modules
EXTRA_CFLAGS += -Wno-unused-value -Wno-unused-variable -Wno-unused-parameter \
		-Wno-switch 

ifeq "$(MACHINE)" "x86_64"
EXTRA_CFLAGS += -Wno-enum-compare
endif

ifneq (,$(findstring 2.4.,$(CURRENT)))
install: 
	@install -d $(KDIR26)
	@for i in usb-it950x.o;do  if [ -e "$$i" ]; then echo -n "$$i "; install -m 644 -c $$i $(KDIR26); fi; done; echo;
else
install:
	@install -d $(KDIR26) 
	@for i in usb-it950x.ko;do  if [ -e "$$i" ]; then echo -n "$$i "; install -m 644 -c $$i $(KDIR26); fi; done; echo;
endif
	@/sbin/depmod -a ${KERNELRELEASE}
#ifeq "$(wildcard $(LMDIR26)/bin)" ""
#	@mkdir $(LMDIR26)/bin
#endif
#	@files='*.bin'; for i in bin/$$files;do if [ -e $(LMDIR26)/$$i ]; then echo -n "$$i "; echo; rm $(LMDIR26)/$$i; fi; done; echo;
#	@for i in bin/*.bin;do  if [ -e "$$i" ]; then echo -n "$$i "; echo; cp -rf bin/*.bin $(LMDIR26)/bin; fi; done; echo;
	@make clean

remove: 
	echo -e "\nRemoving old $(KDIR26)/ files:"
	@files='usb-it950x.ko'; for i in $$files;do if [ -e $(KDIR26)/$$i ]; then echo -n "$$i "; rm $(KDIR26)/$$i; fi; done; for i in $$files;do if [ -e $(KDIR26)/$$i.gz ]; then echo -n "$$i.gz "; rm $(KDIR26)/$$i.gz; fi; done; echo;
#	@files='*.bin'; for i in bin/$$files;do if [ -e $(LMDIR26)/$$i ]; then echo -n "$$i "; echo; rm $(LMDIR26)/$$i; fi; done; echo;
#	@rmdir $(LMDIR26)/bin
	@modprobe -r usb_it950x 

clean:
	@-rm -f *~ *.o *.ko .*.o.cmd .*.ko.cmd *.mod.c *.*~ *.c *.h \
	demux.h dmxdev.h dvb-pll.h dvb-usb-ids.h dvb-usb.h dvb_demux.h dvb_frontend.h \
	dvb_net.h dvb_ringbuffer.h dvbdev.h modules.order Module.markers Module.symvers

releasesrc:
	@rm -rf release
	@mkdir release; mkdir release/it950x_SRC; mkdir release/it950x_SRC/api
	@cp -f api/*.h release/it950x_SRC/api; cp -af src release/it950x_SRC; cp -af .*.o.cmd release/it950x_SRC/api; cp -af *.o release/it950x_SRC/api
	@cp -f Makefile.release release/it950x_SRC; mv release/it950x_SRC/Makefile.release release/it950x_SRC/Makefile
	@rm release/it950x_SRC/api/it950x-*.o; rm release/it950x_SRC/api/dvb-usb-*.o

-include $(KDIR)/Rules.make


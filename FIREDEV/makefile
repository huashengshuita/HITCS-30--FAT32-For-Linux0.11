
LD	       = ld
LDFLAGS	= -Ttext 0xC0000000 -e start


OBJS	= kernel/kernel.o driver/driver.o lib/lib.o
#OBJS	= kernel/kernel.o


all: vmknl


vmknl:$(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o vmknl.o

kernel/kernel.o:
	$(MAKE) -C ./kernel

driver/driver.o:
	$(MAKE) -C ./driver

lib/lib.o:
	$(MAKE) -C ./lib






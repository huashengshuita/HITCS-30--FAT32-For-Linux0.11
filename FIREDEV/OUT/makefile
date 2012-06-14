LD	=ld

OBJS  = head.o main.o page.o asm.o traps.o GRUB.o queue.o\
        vsprintf.o graphicdriver.o vbe.o gui.o font.o console.o\
        bootmem.o timer.o system_call.o pagealloc.o floppy.o\
        keyboard.o mouse.o process.o schedule.o fat12.o harddisk.o\
        fat32.o vmalloc.o syscall.o msgmanger.o
	


kernel.o: $(OBJS)
	$(LD) -o vmknl.o -Ttext 0xC0000000 -e start $(OBJS)
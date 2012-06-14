AS = as 
LD = ld
CC = gcc

CFLAGS   =  -Wall -O2 -g -nostdinc  -I../include
LDFLAGS  =  -g -T misc_ld -nostdlib

OBJS =  head.o misc.o LFB.o


all: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o vmknl.z

head.o: head.s
	 nasm  -f coff head.s


misc.o: misc.c
	$(CC) $(CFLAGS) -c $< -o $@

LFB.o: LFB.c
	$(CC) $(CFLAGS) -c $< -o $@
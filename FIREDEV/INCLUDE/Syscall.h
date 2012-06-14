#ifndef SYSCALL_H
#define SYSCALL_H

typedef struct
{
    //by pusha
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned gs, fs, es, ds;
	unsigned trap_index, error_code;
	unsigned eip, cs, eflags, user_esp, user_ss;
}regs_t;

typedef unsigned arg_t;
#define NULL_ARG	((arg_t) 0)

#define os_info() \
    _system_call(0x00, NULL_ARG, NULL_ARG, NULL_ARG)

#define kLockProc(bLock)\
	  _system_call(0x04, bLock,       NULL_ARG, NULL_ARG)

#define kGetCurInstance()\
	  _system_call(0x05, NULL_ARG,    NULL_ARG, NULL_ARG)

#define kmalloc(size) \
	  _system_call(0x10, size,     NULL_ARG, NULL_ARG)
#define kfree(base,size)\
	  _system_call(0x11, base,     size,     NULL_ARG)

#define kopenfile(filename) \
	  _system_call(0x20, filename,     NULL_ARG, NULL_ARG)
#define kreadfile(fp,buf,size)\
	  _system_call(0x21, fp,     buf,     size)

    
void SystemCallInit(void);
unsigned long _system_call(int eax, unsigned ebx, unsigned ecx, unsigned edx);


#endif

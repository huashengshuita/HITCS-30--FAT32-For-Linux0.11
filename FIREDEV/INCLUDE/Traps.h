
//-----------------------------------------------------------------------------------------------------------------------
//          Traps Header  For FireXOS  1.01 
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                   2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#ifndef TRAPS_H
#define TRAPS_H

typedef struct DescStruct{
	unsigned long a, b;
} DescTable;

extern DescTable idt[256];

#define SetGate(gateAddr,type,dpl,addr) \
__asm__ ("movw %%dx,%%ax\n\t" \
	       "movw %0,%%dx\n\t" \
	       "movl %%eax,%1\n\t" \
	       "movl %%edx,%2" \
	     : \
	     : "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
	       "o" (*((char *) (gateAddr))), \
	       "o" (*(4+(char *) (gateAddr))), \
	       "d" ((char *) (addr)),"a" (0x00100000))

#define SetIntrGate(n,addr)      SetGate(&idt[n],14,0,addr)

#define SetTrapGate(n,addr)      SetGate(&idt[n],15,0,addr)

#define SetSystemGate(n,addr)    SetGate(&idt[n],15,3,addr)

void TrapInit(void);
void KernelPanic(char* str);

void DivideError(void);
void Debug(void);
void Nmi(void);
void Int3(void);
void OverFlow(void);
void Bounds(void);
void InvalidOp(void);
void CoprocessorSegmentOverrun(void);
void Reserved(void);
void DoubleFault(void);
void InvalidTSS(void);
void GeneralProtection(void);
void ParallelInterrupt(void);
void SegmentNotPresent(void);
void PageFault(void);
void StackSegment(void);
void DeviceNotAvailable(void);
void CoprocessorError(void);


#endif

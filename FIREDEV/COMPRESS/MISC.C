
#include "GRUB.h"

void VesaTextOut(unsigned short x,unsigned short y,char * str,unsigned long color);
int  VesaPrintf(unsigned short x,unsigned short y,const char * fmt, ...);
int  LFBInit(GrubInfo* info);

extern unsigned char VmKnl[65536];

int error_boot(void) 
{
  while(1);   
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 打印OK信息
//
//----------------------------------------------------------------------------------------------------------------------------
void PrintOK(unsigned short x,unsigned short y)
{
	VesaTextOut(x,y,"[",0xFFFFFF);
	VesaTextOut(x+8,y,"OK",0x00FF00);
	VesaTextOut(x+24,y,"]",0xFFFFFF);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 保存启动信息
//
//----------------------------------------------------------------------------------------------------------------------------
void SaveTheBootInfo(GrubInfo* BootInfo)
{
	GrubInfo* BootInfoAddr=(GrubInfo *)0x90000;
	*BootInfoAddr=*BootInfo;
	unsigned long* MEMSIZE=(unsigned long*)0x80000;
	*MEMSIZE=BootInfo->mem_upper;
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 内核自解压
//
//----------------------------------------------------------------------------------------------------------------------------
void Decompress(void)
{
	unsigned long  KernelSize  =(unsigned long)0x00017000;    //128KB Kernel
	unsigned char* ToStart     =(unsigned char*)0x00000000;

	register unsigned long  i;


	for(i=0;i<KernelSize;i++)
		*ToStart++=VmKnl[i];
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 内核自解压头入口
//
//----------------------------------------------------------------------------------------------------------------------------
int decompress_kernel(unsigned long cpu_features,unsigned long magic_number,GrubInfo* BootInfo)
{

  LFBInit(BootInfo);
	VesaTextOut(64,  0,   "Welcome to  ",0xFFFFFF);
	VesaTextOut(160, 0,   "FireX OS    ",0xFF0000);
	VesaTextOut(256, 0,   "kernel version is 1.10   CopyRight By XiaoFan 2004~2008",0xFFFFFF);
	VesaTextOut(0,   32,  "Decompress the kernel",0xFFFFFF);
  Decompress();
	PrintOK(800-64,32);
  VesaTextOut(0,48,     "Save the boot infomation",0xFFFFFF);
	SaveTheBootInfo(BootInfo);
	PrintOK(800-64,48);
	VesaTextOut(0,64,     "Jump to  the kernel",0xFFFFFF);
  
	return 1;
}



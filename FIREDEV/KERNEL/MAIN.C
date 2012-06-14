//-----------------------------------------------------------------------------------------------------------------------
//          Kernel Main For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "kernel.h"
#include "GRUB.h"
#include "GraphicDriver.h"
#include "Page.h"
#include "String.h"
#include "Traps.h"
#include "Console.h"
#include "BootMem.h"
#include "Timer.h"
#include "PageAlloc.h"
#include "Queue.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Process.h"
#include "Schedule.h"
#include "Floppy.h"
#include "FS.h"
#include "Msdos.h"
#include "SysCall.h"
#include "Vbe.h"
#include "VMalloc.h"
#include "HardDisk.h"


GrubInfo* GrubBootInfo=(GrubInfo*)0x90000;

struct    bparam_s BootParameters={0,};



//----------------------------------------------------------------------------------------------------------------------------
//
// 内核主入口
//
//----------------------------------------------------------------------------------------------------------------------------
void k_main(void) 
{
	unsigned long  base_mm;
	
	PageDirInit();
	KernelGraphicInit();
	ConsoleInit(0,0xFFFFFF);

  ConsolePrintf("Enable The Page");
	ConsolePrintOK();

	ConsoleMsg("  The Kernel Page    Dir   at %08x The Page0 at %08x\n",PG_DIR_ADDR,PG_TABLE_ADDR);
	ConsoleMsg("  The VESA   Mapping Page  at %08x\n",VESA_PTE);

	ConsolePrintf("Load The boot Infomation");
	ConsolePrintOK();

	GetGrubVersion(GrubBootInfo);
	GetGrubBoot(GrubBootInfo,(int*)&BootParameters.bp_bootdev);
	GetGrubMemInfo(GrubBootInfo,&base_mm,&BootParameters.bp_ramsize);
	GetGrubAPM(GrubBootInfo);

	BootMemInit();
	KnlMesQueInit();

	PageManagerInit(BootParameters.bp_ramsize);
	VMemInit(BootParameters.bp_ramsize,0xC0000000-BootParameters.bp_ramsize);

	TrapInit();
	TimerInit();
	KeyboardInit();
	MouseInit();
	//FloppyInit();
	HardDiskInit();
  //Fat12_Init();
	Fat32_Init();
	ProcessInit();
	SystemCallInit();

	__asm__("sti");

	ExecInitProc("SHELLER ");

  SetSwitching(0);
  while(1);   
}



//-----------------------------------------------------------------------------------------------------------------------
//          Kernel Proc For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "CTYPE.h"
#include "String.h"
#include "Console.h"
#include "Process.h"
#include "PageAlloc.h"
#include "VMalloc.h"
#include "FS.h"
#include "MsgManger.h"
#include "Msdos.h"
#include "SysCall.h"

static unsigned int NextPid=1;

ProcStruct    proc[MAX_PROCESS];
ProcStruct*   ProcCurrent=NULL;
ProcStruct*   ProcHead=NULL;
ProcStruct*   ProcTail=NULL;

//--------------------------------------------------------------------------------------
//
// 获得当前的进程ID号
//
//--------------------------------------------------------------------------------------
int GetPid(void)
{
	NextPid++;

	if (NextPid < MAX_PID)
		return NextPid;
	else{
		NextPid = 1;
		return NextPid;
	}
}

//--------------------------------------------------------------------------------------
//
// 获得空闲进程槽入口
//
//--------------------------------------------------------------------------------------
int GetProcessEntry(void)
{
	register ProcStruct *rp;

	for (rp = &proc[0]; rp < &proc[MAX_PROCESS]; ++rp) {
		if(rp->procFlag == UNUSED)
			return rp->procIndex ;
	}
	return -1;
}


//--------------------------------------------------------------------------------------
//
// 系统核心进程
//
//--------------------------------------------------------------------------------------
void KernelIdle(void)
{
	//ConsolePrintf("KernelIdle1 ...\n");
	MsgMainLoop();
	while(1)
	{
	   
	}
	
}


//--------------------------------------------------------------------------------------
//
// 系统核心进程
//
//--------------------------------------------------------------------------------------
void KernelIdle2(void)
{
	//MsgMainLoop();
	while(1)
	{
	  ConsolePrintf("KernelIdle2 ...\n");
	}
}




//--------------------------------------------------------------------------------------
//
// 设定段描述符
//
//--------------------------------------------------------------------------------------
void SetDesc(SysDesc* _seg,unsigned long base,unsigned long size,unsigned char type)
{
	_seg->baseLow =     base;
	_seg->baseMiddle =  base >> 16;
	_seg->baseHigh =    base >> 24;
	//--size;

	if (size > 0xFFFFFL) {                               //if used visual memory.
		_seg->limitLow =    size >> 12;
		_seg->granularity = 0x80 | (size >> (12 + 16));
	} else {                                             //used physical memory
		_seg->limitLow =    size;
		_seg->granularity = size >> 16;
	}


	//此处G位为1 表示段限单位为 4KB 所以 limitLow =3FFF 时表示段限为 64MB

	_seg->granularity |= 0x40;	                          /* means BIG for data seg */
	_seg->access       =  type;                           //segment type

}


//--------------------------------------------------------------------------------------
//
// 获得段描述符的基址
//
//--------------------------------------------------------------------------------------
unsigned long GetDescBase(SysDesc* _seg)
{
	unsigned long base;
	base = _seg->baseLow | ((unsigned long) _seg->baseMiddle << 16);
	base |= ((unsigned long) _seg->baseHigh << 24);
	return base;
}


//--------------------------------------------------------------------------------------
//
// 设定进程LDT (CHECK OK)
//
//--------------------------------------------------------------------------------------
int MakeProcLdt(ProcStruct *rp)
{
	int nr = rp->procIndex;

	if(nr < 1 || nr > MAX_PROCESS) return 0;

	//每个进程的LDT均为基址为 0 段限为 4GB
	SetDesc(&(rp->procLdt[CS_LDT_INDEX]),0,0xFFFFFFFF, USER_CODE);
	SetDesc(&(rp->procLdt[DS_LDT_INDEX]),0,0xFFFFFFFF, USER_DATA);

	return 1;
}


//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------
int ProcessTssInit(void)
{
	unsigned temp_tss, temp_ldt, nr;
	register ProcStruct* rp;

	for (temp_ldt = FIRST_LDT*DESC_SIZE,temp_tss = FIRST_TSS*DESC_SIZE, 
		rp = &proc[0],  nr=0;   rp < &proc[MAX_PROCESS];
		++rp, ++nr, temp_ldt += 2*DESC_SIZE, temp_tss+=2*DESC_SIZE)
	{
		//SET PROCESS LDT: first is 5.
		SetDesc(&gdt[temp_ldt / DESC_SIZE],(unsigned int)(rp->procLdt),
			      sizeof(rp->procLdt)-1, 0x82);        
		rp->ldtSel = temp_ldt;

		//SET PROCESS TSS: first is 6.
		SetDesc(&gdt[temp_tss / DESC_SIZE],(unsigned int)(&(rp->procTss)),
			      sizeof(rp->procTss)-1, 0x89);

		rp->tssSel =         temp_tss;
		rp->procTss.ldt    = temp_ldt;
		rp->procTss.iobase = 0xffff;                 
		MakeProcLdt(rp);
	}

	return 1;
}


unsigned long SetProcessPageDir(void)
{
	unsigned long pg_dir;
	pg_dir=GetFreePage();

}

//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------
int ProcessInit(void)
{
	register ProcStruct *rp;
	register int t;

	static	struct TssStruct IdleTss = 
	{ 0,0,0x18,0,0,0,0,
	  PG_DIR_ADDR,(unsigned)KernelIdle,0x202,0,0,0,0,0,
	  0,0,0,0x18,0x10,0x18,0x18,0x18,0x18,0,0,0};

	static	struct TssStruct InitTss = 
	{ 0,0,0x18,0,0,0,0,
	  0,0,0x3200,0,0,0,0,0x3FFFF,                                //<-- 用户进程堆栈顶部设在 256 KB 末端
	  0,0,0,0x2B,0x23,0x2B,0x2B,0x2B,0x2B,0,0,0};

  IdleTss.sp0=GetFreePage();
	InitTss.sp0=GetFreePage();
  IdleTss.esp=0x80000;

	InitTss.cr3=__pa(GetFreePage());

	for (rp = &proc[0], t = 0; rp < &proc[MAX_PROCESS]; ++rp, ++t) {
		rp->procFlag = UNUSED;
		rp->procIndex = t;
		rp->priority = 2;
		rp->totalTimeRun=0;
	}

	//copy  first process's tss .
	memcpy((void*)&proc[0].procTss, (void*)&IdleTss, sizeof(IdleTss));
	memcpy((void*)&proc[1].procTss, (void*)&InitTss, sizeof(InitTss));

	proc[0].totalTick=2;
	proc[0].procFlag=P_RUN;
	strcpy(proc[0].procName,"KNL IDEL");
	proc[0].p_next=&proc[1];
	proc[0].pid=0;
	proc[1].totalTick=2;
	proc[1].procFlag=P_RUN; 
	strcpy(proc[1].procName,"INIT");
	proc[1].p_next=&proc[0];
	proc[1].pid=1;
	ProcCurrent=&proc[0];
	ProcHead=&proc[0];
	ProcTail=&proc[1];

	ProcessTssInit();
  //ShowGDTInfo();
	return SUCCESS;
}

//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------
void SetUserPgDir(unsigned long pg_dir)
{
	int index;

	/*
	for(index=0x300;index<0x301;index++)    // 3GB ~ 3GB + 64 MB 对应物理地址 0MB~64MB 只能在核心态访问
	{
      *((unsigned long*)(pg_dir+index*4))=
			(PG_TABLE_ADDR+(index-0x300)*4096)|7;
	}
	*/


	//设置剩下的 4GB-4MB ~ 4GB 空间为显存映射  
	for(index=0x3FF;index<0x400;index++)
	{
    *(unsigned long*)(pg_dir+index*4) =
		 (VESA_PTE+(index-0x3FF)*4096)|7;
	}
}

//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------
void ExecInitProc(char* filename)
{
	FILE*  fp;
	unsigned char* buf;
	unsigned long  read_size=0;

	buf=(unsigned char*)VMalloc(768*4096);
	memset(buf,0,768*4096);

	fp=Fat32_OpenFile(filename);
	buf=(unsigned char*)VMalloc(64*4096);
	memset(buf,0,64*4096);
	read_size=Fat32_Read(fp,buf,fp->f_size);

	memcpy((char*)__va(proc[1].procTss.cr3),(char*)__va(PG_DIR_ADDR),4096);

	*((unsigned long*)__va(proc[1].procTss.cr3))=0;

  AlignPageTableCopy(__va(PG_DIR_ADDR),__va(proc[1].procTss.cr3),(unsigned long)buf,0,64*4096);

	//SetUserPgDir(__va(proc[1].procTss.cr3));
}

//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------
int DoProcessClone (const int nr, regs_t *reg)
{
	register ProcStruct *rp = NULL;
	unsigned char* buf;

	rp =     &proc[nr];
	rp->pid  = GetPid();


	rp->procTss.backlink=0;
	rp->procTss.sp0 = GetFreePage();
	memset((void*)rp->procTss.sp0,0,8*1024);
	rp->procTss.ss0 = 0x18;                               //for kernel use.
	rp->procTss.cr3 = __pa(GetFreePage());
	rp->procTss.eip = reg->eip;
	rp->procTss.eflags = reg->eflags;
	rp->procTss.eax = rp->pid ;                           /*return 0, if entryed new task.*/
	rp->procTss.ecx = reg->ecx;
	rp->procTss.edx = reg->edx;
	rp->procTss.ebx = reg->ebx;
	rp->procTss.esp = reg->user_esp;
	rp->procTss.ebp = reg->ebp;
	rp->procTss.esi = reg->esi;
	rp->procTss.edi = reg->edi;
	rp->procTss.es =  reg->es & 0xffff;
	rp->procTss.cs =  reg->cs & 0xffff;
	rp->procTss.ss =  reg->user_ss & 0xffff;
	rp->procTss.ds =  reg->ds & 0xffff;
	rp->procTss.fs =  reg->fs & 0xffff;
	rp->procTss.gs =  reg->gs & 0xffff;


	rp->totalTick=(unsigned long)reg->ecx;
	rp->p_prev = NULL;
	rp->p_next = NULL;
	rp->p_next_wait = NULL;

	rp->priority = 5;             /*default task priority*/

	rp->ppid = ProcCurrent->pid;

	buf=(unsigned char*)VMalloc(64*4096);
	memset(buf,0,64*4096);
	memcpy(buf,0,64*4096);

	memcpy((char*)__va(rp->procTss.cr3),(char*)__va(ProcCurrent->procTss.cr3),4096);

	*((unsigned long*)__va(rp->procTss.cr3))=0;

	AlignPageTableCopy(__va(ProcCurrent->procTss.cr3),__va(rp->procTss.cr3),(unsigned long)buf,0,64*4096);

	//SetUserPgDir(__va(rp->procTss.cr3));

	strcpy(rp->procName,ProcCurrent->procName);
	ProcTail->p_next=rp;
	rp->p_prev=ProcTail;
	ProcTail=rp;
	rp->p_next=ProcHead;
	rp->procFlag = P_RUN;

	return 0;

}






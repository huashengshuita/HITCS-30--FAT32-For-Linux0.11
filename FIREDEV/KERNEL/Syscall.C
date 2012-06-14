//-----------------------------------------------------------------------------------------------------------------------
//          Kernel SysCall For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "Syscall.h"
#include "Console.h"
#include "VMalloc.h"
#include "Process.h"
#include "Traps.h"
#include "Fs.h"
#include "GUI.h"
#include "MsDos.h"
#include "Schedule.h"
#include "PageAlloc.h"
#include "String.h"
#include "Mouse.h"
#include "PageAlloc.h"

typedef unsigned long (*syscall_fuc)(regs_t);

//----------------------------------------------------------------------------------------------
//
//  空系统调用
//
//----------------------------------------------------------------------------------------------
unsigned long sys_nosys(regs_t reg)
{
  return 0;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 显示OS信息
//
//----------------------------------------------------------------------------------------------
unsigned long sys_info (regs_t reg)
{
	ConsolePrintf("OS System Infomation \n");
	return 0;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 获取当前进程号
//
//----------------------------------------------------------------------------------------------
unsigned long sys_get_cur_instance(regs_t reg)
{
	reg.eax=ProcCurrent->pid;
	return reg.eax;
}

//--------------------------------------------------------------------------------------
//
// 系统调用 Fork
//
//--------------------------------------------------------------------------------------
unsigned long sys_fork(regs_t reg)
{
	int procEntry;

	procEntry = GetProcessEntry();	

	if(procEntry < 0)		return procEntry;
	reg.eax=DoProcessClone(procEntry, &reg);
	return reg.eax;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 核心内存分配
//
//----------------------------------------------------------------------------------------------
unsigned long sys_malloc(regs_t reg)
{
  unsigned long size=reg.ebx;
	reg.eax=VMalloc(size);
	//ConsoleMsg("VMalloc a Mem Addr %08x Size %08x\n",reg.eax,size);
	return reg.eax;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 核心内存释放
//
//----------------------------------------------------------------------------------------------
unsigned long sys_free(regs_t reg)
{
	unsigned long base=reg.ebx;
	unsigned long size=reg.ecx;
	VFree(base,size);
	//ConsoleMsg("Free a Mem Addr %08x Size%08x\n",reg.ebx,size);
	return 0;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 获取物理内存页面信息
//
//----------------------------------------------------------------------------------------------
unsigned long sys_page_info(regs_t reg)
{
	int* pTotalPage=(int*)reg.ebx;
	int* pFreePage= (int*)reg.ecx;

	*pTotalPage=(int)PAGING_PAGES;
	*pFreePage =PageManagerScan();

	return 0;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 获取虚拟内存空洞信息
//
//----------------------------------------------------------------------------------------------
unsigned long sys_mem_hole_info(regs_t reg)
{
	unsigned long* pHoleSize= (unsigned long*)reg.ebx;
	unsigned long* pHoleNum = (unsigned long*)reg.ecx;

	unsigned long  size,num;

	VMemScan(&size,&num);

	*pHoleSize=size;
	*pHoleNum =num;

	return 0;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 注册窗口
//
//----------------------------------------------------------------------------------------------
unsigned long sys_register_win(regs_t reg)
{
	WINSTRUCT* pWin;

	unsigned long  pid=ProcCurrent->pid;

	pWin=(WINSTRUCT*)reg.ebx;
	pWin->hWnd+=(pid<<16);

	AddWinSoltToArray(pWin);
  
	//ConsolePrintf("Register a win hWnd = %08x",pWin->hWnd);
	reg.eax=pWin->hWnd;
	return reg.eax;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 获取DC
//
//----------------------------------------------------------------------------------------------
unsigned long sys_get_dc(regs_t reg)
{
	HDC      hdc;
	HWND     hWnd=reg.ebx;
	hdc=GetDC(hWnd);

	reg.eax=(unsigned long)hdc;
	return reg.eax;

}


//----------------------------------------------------------------------------------------------
//
//  系统调用 设定窗体位置
//
//----------------------------------------------------------------------------------------------
unsigned long sys_set_win_pos(regs_t reg)
{
	HWND     hWnd=reg.ebx;
	POINT*   pt;
  int      x,y;

	pt=(POINT*)reg.ecx;

	x=pt->x;
	y=pt->y;
  
	reg.eax=SetWndPos(hWnd,x,y);
	return reg.eax;
}



//----------------------------------------------------------------------------------------------
//
//  系统调用 设定窗体大小
//
//----------------------------------------------------------------------------------------------
unsigned long sys_set_win_size(regs_t reg)
{

	HWND     hWnd=reg.ebx;
  int      w,h;

	w=reg.ecx;
	h=reg.edx;

	reg.eax=SetWndSize(hWnd,w,h);
	return reg.eax;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 获得窗体大小
//
//----------------------------------------------------------------------------------------------
unsigned long sys_get_win_rect(regs_t reg)
{
	HWND     hWnd=reg.ebx;
	RECT     rc;
	RECT*    pRect;

	pRect=(RECT*)(reg.ecx);

	rc=GetWinRect(hWnd);

	memcpy(pRect,&rc,sizeof(RECT));

	reg.eax=0;
	return reg.eax;

}


//----------------------------------------------------------------------------------------------
//
//  系统调用 设置指定窗体以下所有窗体的剪裁域
//
//----------------------------------------------------------------------------------------------
unsigned long sys_set_below_cliprgn(regs_t reg)
{
	HWND     hWnd=reg.ebx;

	SetBelowClipRgn(hWnd);

	return 1;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 设置指定窗体以上所有窗体的剪裁域
//
//----------------------------------------------------------------------------------------------
unsigned long sys_set_above_cliprgn(regs_t reg)
{
	HWND     hWnd=reg.ebx;

	SetAboveClipRgn(hWnd);

	return 1;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 设置指定窗体以下所有窗体的刷新域
//
//----------------------------------------------------------------------------------------------
unsigned long sys_set_below_invrgn(regs_t reg)
{
	HWND     hWnd=reg.ebx;
	RECT     rc;
	RECT*    pRect;

	pRect=(RECT*)(reg.ecx);

	memcpy(&rc,pRect,sizeof(RECT));
	
	SetBelowInvalidRng(hWnd,rc);

	return 1;

}

//----------------------------------------------------------------------------------------------
//
//  系统调用打开文件
//
//----------------------------------------------------------------------------------------------
unsigned long sys_fileopen(regs_t reg)
{
	FILE* fp=NULL;
	unsigned char* filename=(unsigned char*)reg.ebx;
	fp=Fat32_OpenFile(filename);
	reg.eax=(unsigned long)fp;
	return reg.eax;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用读文件
//
//----------------------------------------------------------------------------------------------
unsigned long sys_fileread(regs_t reg)
{
	FILE* fp=(FILE*)reg.ebx;
	unsigned char* buf=(unsigned char*)reg.ecx;
	unsigned long  size=reg.edx;

	reg.eax=Fat32_Read(fp,buf,size);

	return reg.eax;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 获取窗体进程号
//
//----------------------------------------------------------------------------------------------
unsigned long sys_get_win_instance(regs_t reg)
{
	int i;
  HWND     hWnd=reg.ebx;

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			reg.eax=WinArray[i].win.hInstance;
			return reg.eax;
		}
	}

	reg.eax=0;
	return reg.eax;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 获取窗体消息队列
//
//----------------------------------------------------------------------------------------------
unsigned long sys_get_message_queue(regs_t reg)
{
	register int i;
	unsigned long pMsgQueue;
	HWND     hWnd=reg.ebx;

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			pMsgQueue=(unsigned long)(WinArray[i].win.MessageQueue);
			reg.eax=pMsgQueue;
		  return reg.eax;
	  }
	}
	reg.eax=NULL;
	return reg.eax;

}

//----------------------------------------------------------------------------------------------
//
//  系统调用 获取窗体消息处理函数指针
//
//----------------------------------------------------------------------------------------------
unsigned long sys_get_winproc(regs_t reg)
{
	register int i;
	unsigned long pWndProc;
	HWND     hWnd=reg.ebx;

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			pWndProc=(unsigned long)(WinArray[i].win.WindowProc);
			reg.eax=pWndProc;
		  return reg.eax;
	  }
	}
	reg.eax=NULL;
	return reg.eax;


}


//----------------------------------------------------------------------------------------------
//
//  系统调用 向指定窗体发送消息
//
//----------------------------------------------------------------------------------------------
unsigned long sys_send_message(regs_t reg)
{
	int i;
	KnlMes  mes;
	KnlMes* pMes=(KnlMes*)(reg.ecx);
	HWND    hWnd=reg.ebx;

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{
			memcpy(&mes,pMes,sizeof(KnlMes));
			KnlPutMessage(WinArray[i].win.MessageQueue,mes);
			reg.eax=1;
			return reg.eax;
		}
	}

	reg.eax=0;
	return reg.eax;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 刷新鼠标缓冲
//
//----------------------------------------------------------------------------------------------
unsigned long sys_refresh_ms_buf(regs_t reg)
{
	RECT  aInvRect;
	RECT* pRect;
	pRect=(RECT*)(reg.ebx);
	memcpy(&aInvRect,pRect,sizeof(RECT));
	RefreshMouseBuf(aInvRect);
	reg.eax=0;
	return reg.eax;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 锁定当前进程
//
//----------------------------------------------------------------------------------------------
unsigned long sys_lock_proc(regs_t reg)
{
	unsigned char bLock=(unsigned char)reg.ebx;
	SetSwitching(bLock);
	reg.eax=0;
	return reg.eax;
}


//----------------------------------------------------------------------------------------------
//
//  系统调用 阻塞当前进程
//
//----------------------------------------------------------------------------------------------
unsigned long sys_block_proc(regs_t reg)
{
	int                     i;
	ProcStruct              *rp;
	HINSTANCE               hInstance=reg.ebx;

	for (rp=&proc[0],i=0;rp<&proc[MAX_PROCESS];++rp,++i)
	{
		if(rp->pid==hInstance)
		{
			rp->procFlag=P_BLOCK;
		  rp->totalTick=0;
			Schedule();
			return TRUE;
		}
	}
  
	return FALSE;
}

//----------------------------------------------------------------------------------------------
//
//  系统调用 唤醒进程
//
//----------------------------------------------------------------------------------------------
unsigned long sys_wakeup_proc(regs_t reg)
{
	int                     i;
	ProcStruct              *rp;
	HINSTANCE               hInstance=reg.ebx;

	for (rp=&proc[0],i=0;rp<&proc[MAX_PROCESS];++rp,++i)
	{
		if(rp->pid==hInstance)
		{
			rp->procFlag=P_RUN;
		  rp->totalTick=10;
			Schedule();
			return TRUE;
		}
	}
  
	return FALSE;
}



syscall_fuc	syscall_table[5][16] = {
/*
**0x0X: kernel
*/              {
               sys_info , sys_fork , sys_nosys, sys_nosys,
        	     sys_lock_proc, sys_get_cur_instance, sys_block_proc, sys_wakeup_proc,
	             sys_nosys, sys_nosys, sys_nosys, sys_nosys,
	             sys_nosys, sys_nosys, sys_nosys, sys_nosys,
                 },
/*
**0x1X: mm call
*/             {
               sys_malloc, sys_free,  sys_page_info, sys_mem_hole_info,
        	     sys_nosys,  sys_nosys, sys_nosys, sys_nosys,
	             sys_nosys,  sys_nosys, sys_nosys, sys_nosys,
	             sys_nosys,  sys_nosys, sys_nosys, sys_nosys,
                 },
/*
**0x2X: file system call
*/             {
               sys_fileopen, sys_fileread, sys_nosys, sys_nosys,
        	     sys_nosys, sys_nosys, sys_nosys, sys_nosys,
	             sys_nosys, sys_nosys, sys_nosys, sys_nosys,
	             sys_nosys, sys_nosys, sys_nosys, sys_nosys,
                 },
/*
**0x3X: API
*/            {
	             sys_register_win, sys_nosys, sys_nosys, sys_nosys,
        	     sys_send_message, sys_nosys, sys_nosys, sys_nosys,
	             sys_refresh_ms_buf, sys_nosys, sys_nosys, sys_nosys,
	             sys_nosys, sys_nosys, sys_nosys, sys_nosys,
                 },
/*
**0x4X: windows call
*/             {
               sys_get_dc, sys_get_message_queue, sys_get_winproc, sys_get_win_instance,
        	     sys_set_win_pos, sys_set_win_size, sys_get_win_rect, sys_nosys,
	             sys_set_below_cliprgn, sys_set_below_invrgn, sys_set_above_cliprgn, sys_nosys,
	             sys_nosys, sys_nosys, sys_nosys, sys_nosys,
                 },
};


#define NR_SYSCALL 0x50


unsigned long _system_call(int eax, unsigned ebx, unsigned ecx, unsigned edx)
{
	unsigned long ret_val;

	__asm__ __volatile__("int %1\n"
		: "=a"(ret_val)		/* EAX	    EBX		ECX		EDX */
		: "i"(0x80), "a"((unsigned)eax), "b"(ebx), "c"(ecx), "d"(edx));

	return ret_val;
}


extern int sys_call(void);


void SystemCallInit(void)
{
	 ConsolePrintf("Set the system call");
	 SetSystemGate(0x80,&sys_call);
	 ConsolePrintOK();
}

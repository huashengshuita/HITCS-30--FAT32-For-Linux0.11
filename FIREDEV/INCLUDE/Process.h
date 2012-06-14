#ifndef  PROCESS_H
#define  PROCESS_H

#include "Page.h"
#include "Syscall.h"

#define USER_CODE  0xfa
#define USER_DATA  0xf2

#define DESC_SIZE 8

#define UNUSED  0 

#define P_RUN      1
#define P_BLOCK    2
#define P_READY    3

#define SUCCESS 1

#define LIMIT_1M 0xfffff
#define LIMIT_8M 0x7fffff
#define LIMIT_4G 0xfffffff


#define FIRST_LDT 6
#define FIRST_TSS 7

#define PROC_MEM_ADDR        		0x400000L   //process memory:from 4 MB virtual addr
#define PROC_MEM_SIZE        		0x40000L    //进程内存大小 256 KB
#define PROC_MEM_BASE(n)        (0xC0000000+PROC_MEM_ADDR+n*PROC_MEM_SIZE) //基地址

#define PROC_KSTACK_ADDR      	0xC0150000L   //进程核心堆栈基址
#define PROC_KSTACK_SIZE        0x1000L       //进程核心堆栈大小   4KB
#define PROC_KSTACK_BASE(n)     (PROC_KSTACK_ADDR+(n)*PROC_KSTACK_SIZE)
#define PROC_KSTACK_TOP(n)      (PROC_KSTACK_ADDR+(n+1)*PROC_KSTACK_SIZE)

#define CS_LDT_INDEX     	1	/* process CS */
#define DS_LDT_INDEX     	2	/* process DS=ES=FS=GS=SS */

#define LDT_CS		        ((CS_LDT_INDEX<<3)|0x07)
#define LDT_DS		        ((DS_LDT_INDEX<<3)|0x07)

#define MAX_PID      20000
#define MAX_PROCESS  64



typedef struct ProcStruct
{
	unsigned need_schedule;
	unsigned priority;

	unsigned ldtSel;
	SysDesc  procLdt[3];	        //进程LDT

	unsigned tssSel;
	struct   TssStruct procTss;		//进程TSS

	int procIndex,totalTick, exitCode;
	unsigned long totalTimeRun;
	int pid, ppid ;                    //process 's parent

	char  procName[32];
	int   procFlag;


	struct ProcStruct *p_next;           //下一个就绪进程
	struct ProcStruct *p_prev;           //下一个准备进程
	struct ProcStruct *p_next_wait;      //下一个准备进程


}ProcStruct;

extern ProcStruct    proc[MAX_PROCESS];
extern ProcStruct*   ProcCurrent;
extern ProcStruct*   ProcHead;
extern ProcStruct*   ProcTail;

int  ProcessInit(void);
void ExecInitProc(char* filename);
unsigned long GetDescBase(SysDesc* _seg);
int  GetProcessEntry(void);
int  DoProcessClone (const int nr, regs_t *reg);

#endif


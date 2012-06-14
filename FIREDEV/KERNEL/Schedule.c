
//-----------------------------------------------------------------------------------------------------------------------
//          Kernel Schedule For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "CTYPE.h"
#include "Process.h"
#include "Traps.h"
#include "Console.h"

int  ProcSwitching=1;

static  long  clockTick=0;


//----------------------------------------------------------------------------------------------------------------------------------------
//
// 跳转到TSS段选择符指定的进程去
//
//----------------------------------------------------------------------------------------------------------------------------------------
void JumpTss (unsigned short tss_sel)
{
   unsigned char buf[6]; /*selector 48bit*/

   if(tss_sel < FIRST_TSS*8 || tss_sel > (FIRST_TSS+MAX_PROCESS*2)*8 || tss_sel%8 != 0){
	    KernelPanic("The TSS SEL IS BAD !!!");
   }
    buf[0]=0;
    buf[1]=0;
    buf[2]=0;
    buf[3]=0;
    buf[4]=tss_sel & 255;
    buf[5]=tss_sel >> 8;

		
	__asm__("sti");
	ProcSwitching=0;

	//asm ("ljmp (%%eax)": : "a" (&buf));
  asm ("ljmp *(%0)": :"g" (&buf));
}


//----------------------------------------------------------------------------------------------------------------------------------------
//
// 进程调度主函数
//
//----------------------------------------------------------------------------------------------------------------------------------------
int Schedule(void)
{

	clockTick++;
	ProcCurrent->totalTimeRun++;

	if(ProcSwitching) return 0;

	//ConsolePrintf("Schedule ProcessSwitching = %d\n",ProcSwitching);

	if (clockTick >= ProcCurrent->totalTick) {

		  clockTick=0;

	    ProcSwitching=0;   

	if (ProcCurrent==NULL) return -1;

	if (ProcCurrent->p_next == NULL) { // 只有这一个进程运行，死机
    KernelPanic("NEXT NULL");      
		return 1;
	}
  
	//ConsolePrintf("Proc Jmp next\n"); 

	ProcCurrent = ProcCurrent->p_next; // 转到下一个进程


	JumpTss(ProcCurrent->tssSel);      //run this process	

	}
	return SUCCESS;
}



void SetSwitching(int bSwitch)
{
    ProcSwitching=bSwitch;
}


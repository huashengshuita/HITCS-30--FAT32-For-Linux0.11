
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
// ��ת��TSS��ѡ���ָ���Ľ���ȥ
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
// ���̵���������
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

	if (ProcCurrent->p_next == NULL) { // ֻ����һ���������У�����
    KernelPanic("NEXT NULL");      
		return 1;
	}
  
	//ConsolePrintf("Proc Jmp next\n"); 

	ProcCurrent = ProcCurrent->p_next; // ת����һ������


	JumpTss(ProcCurrent->tssSel);      //run this process	

	}
	return SUCCESS;
}



void SetSwitching(int bSwitch)
{
    ProcSwitching=bSwitch;
}


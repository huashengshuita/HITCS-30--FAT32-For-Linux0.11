//-----------------------------------------------------------------------------------------------------------------------
//          Timer Driver For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                   2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "Timer.h"
#include "Queue.h"
#include "Message.h"
#include "Console.h"
#include "Io.h"
#include "Traps.h"
#include "Schedule.h"


#define BCD_TO_BIN(val) ((val)=((val)&15)+((val)>>4)*10);

#define HZ 2000	                                                           //希望的时钟中断频率，每秒钟400次
#define LATCH (1193182 / HZ)

static unsigned long jiffies=0;
static unsigned long ntick=0;

CmosTime       CurrentTime;                                                //当前时间
CmosDate       CurrentDate;
 
void TimerInterrupt(void);                                      //时间中断句柄



void TickDelay(int sec)
{
	unsigned long NewDelayTick,OldDelayTick;
 
	__asm__("sti");
	OldDelayTick=NewDelayTick=jiffies;

	while(NewDelayTick-OldDelayTick<1000*sec)
	{
		NewDelayTick=jiffies;
		ConsolePrintf("");
	}
	__asm__("cli");
}

//--------------------------------------------------------------------------------------------------------------------------
//
//  获得当前的Tick值
//
//--------------------------------------------------------------------------------------------------------------------------
unsigned long GetCurrentTick(void)
{
	return jiffies;
}

//--------------------------------------------------------------------------------------------------------------------------
//
// 初始化时钟中断
//
//--------------------------------------------------------------------------------------------------------------------------
void TimerInit(void)
{
	//设置中断频率
	outb(0x34, 0x43);	                                /* binary, mode 2, LSB/MSB, ch 0 */
	outb(LATCH & 0xFF, 0x40);	                        /* LSB */
	outb(LATCH >> 8, 0x40);	                          /* MSB */
  
	ConsolePrintf("Set the timer driver");

	GetCmosTime(&CurrentTime);
	GetCmosDate(&CurrentDate);

	SetIntrGate(0x20,&TimerInterrupt);                //安装时钟中断处理函数
	outb(inb_p(0x21)&~0x01,0x21);                     //允许时钟中断

	ConsolePrintOK();

	ConsoleMsg("\n");
	ConsoleMsg("  The Timer Frequency is %d HZ\n",HZ);
	ConsoleMsg("  The Start PC Time   is %02d:%02d:%02d %04d-%02d-%02d\n",
		            CurrentTime.hour,CurrentTime.min,CurrentTime.sec,
								CurrentDate.year,CurrentDate.month,CurrentDate.day);
  ConsoleMsg("\n");

	TickDelay(2);
}



//---------------------------------------------------------------------------------------------------------------------------
//
// 获得 CMOS 时间
//
//---------------------------------------------------------------------------------------------------------------------------
void GetCmosTime(CmosTime* t)
{
	if (t) {
		t->sec =   ReadCmos(0);
		t->min =  ReadCmos(2);
		t->hour=  ReadCmos(4);
	}
	BCD_TO_BIN(t->sec);
	BCD_TO_BIN(t->min);
	BCD_TO_BIN(t->hour);

}

//-----------------------------------------------------------------------------------------------------------------------------
//
//获得 CMOS 日期
//
//-----------------------------------------------------------------------------------------------------------------------------
void GetCmosDate(CmosDate* d)
{
	if (d) {
		d->dayofweek =ReadCmos(6);
		d->day =ReadCmos(7);
		d->month =ReadCmos(8);
		d->year = ReadCmos(9);
	}
	BCD_TO_BIN(d->day);
	BCD_TO_BIN(d->month);
	BCD_TO_BIN(d->year);
	d->year+=2000;

}


//----------------------------------------------------------------------------------------------------------------------------------------
//
// 时间中断句柄
//
//----------------------------------------------------------------------------------------------------------------------------------------
void TimerHandle(long cpl)
{
  KnlMes mes;
	jiffies++;       
	ntick++;

	if(ntick>200)
	{
		mes.MessageType=MES_TIMER;
		KnlPutMessage(KnlMesQue,mes);
		ntick=0;
	}

	Schedule();

}


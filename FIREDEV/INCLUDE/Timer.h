
//-----------------------------------------------------------------------------------------------------------------------
//          Timer Driver Header For FireXOS  1.01 
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                   2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#ifndef TIMER_H
#define TIMER_H
 
/*---------------时间结构----------------------------------*/
typedef struct  {
	unsigned char sec, min, hour;
}CmosTime;

/*---------------日期结构----------------------------------*/
typedef struct  {
	int dayofweek, day, month, year;
}CmosDate;

//---------------------------------------------------------------------------------------------------------------------------------------------
// 时钟驱动接口函数
//---------------------------------------------------------------------------------------------------------------------------------------------

extern CmosTime currentTime;

void TimerInit(void);                     

unsigned long GetCurrentTick(void);

void TickDelay(int sec);

void GetCmosTime(CmosTime* t);

void GetCmosDate(CmosDate* d);

unsigned long GetTick(void);


#endif

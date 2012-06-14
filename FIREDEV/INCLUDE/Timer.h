
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
 
/*---------------ʱ��ṹ----------------------------------*/
typedef struct  {
	unsigned char sec, min, hour;
}CmosTime;

/*---------------���ڽṹ----------------------------------*/
typedef struct  {
	int dayofweek, day, month, year;
}CmosDate;

//---------------------------------------------------------------------------------------------------------------------------------------------
// ʱ�������ӿں���
//---------------------------------------------------------------------------------------------------------------------------------------------

extern CmosTime currentTime;

void TimerInit(void);                     

unsigned long GetCurrentTick(void);

void TickDelay(int sec);

void GetCmosTime(CmosTime* t);

void GetCmosDate(CmosDate* d);

unsigned long GetTick(void);


#endif

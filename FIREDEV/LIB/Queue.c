//-----------------------------------------------------------------------------------------------------------------------
//          Kernel Queue Lib For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "BootMem.h"
#include "Message.h"
#include "Queue.h"
#include "Console.h"

KnlQueue* KnlMesQue;
KnlMes KernelMessage[MesMaxLength];

void SetQueue(KnlQueue* pQueue,KnlMes* mesaddr,int n);
void ClearQueue(KnlQueue* pQueue);



/*--------------------------核心消息队列缓冲区初始化--------------------------------------------------------*/
void KnlMesQueInit(void)
{
  unsigned long size;
	ConsolePrintf("Kernel message queue init");
	size=sizeof(KnlQueue);
  if((size % 4096)!=0) size+=(4096-(size % 4096));
	KnlMesQue=(KnlQueue* )BootMemMalloc(size);
  SetQueue(KnlMesQue,KernelMessage,MesMaxLength);
	ConsolePrintOK();
}



/*-------------------------构造函式。建立数组长度为n的空队列-------------------------------------------------*/
void SetQueue(KnlQueue* pQueue,KnlMes* mesaddr,int n)
{
	pQueue->data=mesaddr;
	pQueue->max=n;
	pQueue->front=0;
	pQueue->rear=0;
	pQueue->size=0;
	ClearQueue(pQueue);
}

/*---------------------------------判空。为空返回true---------------------------------------------------------*/
unsigned char QueueEmpty(KnlQueue* pQueue)
{
    if(pQueue->size==0)
    return true;
    else return false;
}

/*-------------------------------------判满。为满返回true-----------------------------------------------------*/
unsigned char QueueFull(KnlQueue* pQueue)
{
    if(pQueue->size==pQueue->max)
    return true;
    else return false;
}


/*-----------------------------------求队列长度----------------------------------------------------------------*/
int QueueSize(KnlQueue* pQueue)
{
    return (pQueue->size);
}


/*--------------------------------------入队-------------------------------------------------------------------*/
void KnlPutMessage(KnlQueue* pQueue,KnlMes mes)
{
	if(QueueFull(pQueue)) return;
	pQueue->data[pQueue->rear]=mes;
	pQueue->rear=(pQueue->rear+1)%(pQueue->max);
	pQueue->size++;
}

/*-------------------------------------出队---------------------------------------------------------------------*/
KnlMes KnlGetMessage(KnlQueue* pQueue)
{
	KnlMes mes;
	if(pQueue->size>0)
  {
	   mes=pQueue->data[pQueue->front];
	   pQueue->front=(pQueue->front+1)%(pQueue->max);
	   pQueue->size--;
	   return mes;        
  }
	return mes;
}

/*------------------------------------清队列--------------------------------------------------------------------*/  
void ClearQueue(KnlQueue* pQueue)
{
	pQueue->front=0;
	pQueue->rear=0;
	pQueue->size=0;
}

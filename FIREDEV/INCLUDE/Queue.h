#ifndef QUEUE_H
#define QUEUE_H

#include "Message.h"
#include "CTYPE.h"

#define  true  1
#define  false 0
#define  NULL  0

typedef struct _KnlQueue
{
		 KnlMes *data;
	   int max;
		 int front;
		 int rear;
		 int size;
}KnlQueue;



extern KnlQueue* KnlMesQue;

void   KnlMesQueInit(void);
KnlMes KnlGetMessage(KnlQueue* pQueue);
void   KnlPutMessage(KnlQueue* pQueue,KnlMes mes);
unsigned char QueueEmpty(KnlQueue* pQueue);




#endif

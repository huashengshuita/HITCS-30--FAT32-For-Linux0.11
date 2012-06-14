//-----------------------------------------------------------------------------------------------------------------------
//          MsgManger For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "Mouse.h"
#include "Queue.h"
#include "Message.h"
#include "MsgManger.h"
#include "Console.h"
#include "Schedule.h"


int bPressed=0;
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MsgMainLoop(void)
{
	int         xChange,yChange;
	KnlMes      mes;
	WINSTRUCT*  pWin=NULL;
	//WINSTRUCT*  pWin2=NULL;
	int         x,y,w,h;
	HWND        hTopWnd=NULL;
	//int         WL,WT,WR,WB;

	InitMouseBuf();

	while(1)
	{
		if(!QueueEmpty(KnlMesQue))
		 {		  
			mes=KnlGetMessage(KnlMesQue);
			switch(mes.MessageType)
			{
			case MES_KEYUP:
				//KnlSendMessage(0x00020019,mes);
			break;

			case MES_KEYDOWN:

        mes.MessageType=MES_KEYDOWN;
				BroadCastMessage(mes);

			break;


			case MES_MOUSE_MOVE:

				xChange=mes.Param1;
				yChange=mes.Param2;

				//ConsolePrintf("MOVE ");
				SetMousePos(&xChange,&yChange);

				if(bPressed && hTopWnd!=NULL)

				{
				pWin=KnlGetWndPtr(hTopWnd);

				x=pWin->winOldRect.left;
        y=pWin->winOldRect.top;
				w=pWin->winOldRect.right-pWin->winOldRect.left;
				h=pWin->winOldRect.bottom-pWin->winOldRect.top;

				mes.MessageType=MES_WND_MOVE;

				mes.Param1=SETLONGWORD(x+xChange,y+yChange);
				mes.Param2=SETLONGWORD(w,h);

				pWin->winOldRect.left+=xChange;
				pWin->winOldRect.top+=yChange;
				pWin->winOldRect.right+=xChange;
				pWin->winOldRect.bottom+=yChange;

				KnlSendMessage(hTopWnd,mes);
				}

			break;

			case MES_MOUSE_LDOWN:

				
				x=LWORD(mes.Param1);
				y=HWORD(mes.Param1);

				//ConsolePrintf("LDown x %d y %d\n",x,y);

        if(hTopWnd!=GetTopMainWin(x,y))
				{
				   hTopWnd=GetTopMainWin(x,y);
					 //ConsolePrintf("Get Top Wnd hWnd %08x",hTopWnd);
				   SetActiveWindow(hTopWnd);
				}

				if(bPressed) bPressed=0;
				        else bPressed=1;

				/*
				if(hTopWnd!=NULL)
				{
				   pWin2=GetCurWinSolt(hTopWnd);
					 WL=pWin2->winRect.left;
					 WR=pWin2->winRect.right;
					 WT=pWin2->winRect.top;
					 WB=pWin2->winRect.bottom;
  
					 //SetSwitching(TRUE);
				   if(x>=WL+20 && x<WR-50)
				   {		  
				        if(bPressed) bPressed=0;
				        else bPressed=1;
				   }
					 //SetSwitching(FALSE);
				}
				*/

			break;

			case MES_MOUSE_RDOWN:

        mes.MessageType=MES_MOUSE_RDOWN;
				BroadCastMessage(mes);

			break;

			case MES_TIMER:

        mes.MessageType=MES_TIMER;
				BroadCastMessage(mes);

			break;


			}
		}
	

	}
}

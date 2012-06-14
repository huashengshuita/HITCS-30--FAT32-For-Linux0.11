//-----------------------------------------------------------------------------------------------------------------------
//          Kernel GUI LIB For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "GUI.h"
#include "CTYPE.h"
#include "String.h"
#include "Console.h"
#include "GraphicDriver.h"
#include "Schedule.h"

WINSLOT WinArray[MAX_WINSLOT];

static  HWND        hWinTail=0;
static  HWND        hWinHead=0;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
WINSTRUCT*  GetCurWinSolt(HWND hWnd)
{
	register int i;

	for(i=0;i<MAX_WINSLOT ;i++)
	{
		    if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
				{
					return &WinArray[i].win;
				}
	}

	return NULL;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
HWND GetNextWinSolt(HWND hWnd)
{
	register int i;

	for(i=0;i<MAX_WINSLOT ;i++)
	{
		    if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
				{
					return WinArray[i].win.hWinNext;
				}
	}
	return NULL;

}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
HWND GetPrevWinSolt(HWND hWnd)
{
	register int i;

	for(i=0;i<MAX_WINSLOT ;i++)
	{
		    if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
				{

					return WinArray[i].win.hWinPrev;
				}
	}

	return NULL;

}


//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
BOOL IsInRect(int x,int y,RECT rc)
{
	if(x>=rc.left && x<rc.right && y>=rc.top && y<rc.bottom)
	return TRUE;
	else
	return FALSE;
	
}


//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
HWND GetTopMainWin(int x,int y)
{
	WINSTRUCT* pWin=GetCurWinSolt(hWinTail);

  
	//ConsolePrintf("Get Top Wnd x %d y %d pWin->winRect.left %d\n",x,y,pWin->winRect.left);

	while(pWin->hWnd!=hWinHead)
	{

		if(x>=pWin->winRect.left && x<pWin->winRect.right 
			 && y>=pWin->winRect.top && y<pWin->winRect.bottom)
		if(pWin->winStyle==eWinNormal)
		return pWin->hWnd;

		//ConsolePrintf("pWin hWnd %08x Get Prive Wnd %08x",pWin->hWnd,pWin->hWinPrev);
		pWin=GetCurWinSolt(pWin->hWinPrev);

	}

	return NULL;
}

void SendInvalidRect(HWND hWnd,RECT InvRect);
//-----------------------------------------------------------------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SetTopWindow(HWND hWnd)
{
	WINSTRUCT*  pWin=GetCurWinSolt(hWnd);
	WINSTRUCT*  pWinPrev=GetCurWinSolt(pWin->hWinPrev);
	WINSTRUCT*  pWinNext=GetCurWinSolt(pWin->hWinNext);
	WINSTRUCT*  pWinTail=GetCurWinSolt(hWinTail);
	int         w,h;

	w=pWin->winRect.right-pWin->winRect.left;
	h=pWin->winRect.bottom-pWin->winRect.top;

	if(hWnd==0 || hWnd==hWinTail) return FALSE;
	
	//pWinTail->hWinNext=pWin->hWinNext;
	pWinPrev->hWinNext=pWin->hWinNext;
	pWinNext->hWinPrev=pWin->hWinPrev;

	pWin->hWinPrev=pWinTail->hWinPrev;
	pWin->hWinNext=hWinTail;

	GetCurWinSolt(pWinTail->hWinPrev)->hWinNext=hWnd;
	pWinTail->hWinPrev=hWnd;

	memset(&pWin->winHDC->ecrgn,0,sizeof(CLIPRGN));
	SetBelowClipRgn(hWnd);
	SetAboveClipRgn(hWnd);

	//InvRect=SetRect(0,0,w,h);
	//SendInvalidRect(hWnd,InvRect);

	return TRUE;

}


//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
WINSTRUCT* KnlGetWndPtr(HWND hWnd)
{
	int i;
	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			 return &(WinArray[i].win);
		}
	}
	return NULL;

}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL SetWndSize(HWND hWnd,int w,int h)
{
	int i,x,y;
	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			x=WinArray[i].win.winRect.left;
			y=WinArray[i].win.winRect.top;
			WinArray[i].win.winRect=SetRect(x,y,x+w,y+h);
			WinArray[i].win.winHDC->Bounds=SetRect(x,y,x+w,y+h);
			//KernelPrintf(0,32+16,"SetWinSize x=%d y=%d W=%d H=%d",x,y,w,h);
			//size_nr++;
		  return TRUE;
	  }
	}
	return FALSE;
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL SetWndPos(HWND hWnd,int x,int y)
{
	int i,w,h;
	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			w=WinArray[i].win.winRect.right-WinArray[i].win.winRect.left;
			h=WinArray[i].win.winRect.bottom-WinArray[i].win.winRect.top;
			WinArray[i].win.winRect=SetRect(x,y,x+w,y+h);
			WinArray[i].win.winHDC->Bounds=SetRect(x,y,x+w,y+h);
		  return TRUE;
	  }
	}
	return FALSE;
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
int KnlSendMessage(HWND hWnd,KnlMes mes)
{
	int i;

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{
			mes.hInstance=0;
			mes.hWnd=hWnd;
			KnlPutMessage(WinArray[i].win.MessageQueue,mes);
			//ProcCurrent->procFlag=P_BLOCK;
			//ProcCurrent->totalTick=0;
			//Schedule();
      return 1;
		}
	}

	return 0;
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void BroadCastMessage(KnlMes mes)
{
	register int i;
	for(i=0;i<MAX_WINSLOT ;i++)
	{
		    if(WinArray[i].bUsed)
					KnlSendMessage(WinArray[i].win.hWnd,mes);					
	}

}



//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL AddWinSoltToArray(WINSTRUCT* pWin)
{
  int i;
	for(i=0;i<MAX_WINSLOT ;i++)
		if(WinArray[i].bUsed==FALSE)
		{ 
			 if(hWinHead==NULL)
			 {
				 pWin->hWinPrev=NULL;
	       pWin->hWinNext=NULL;
				 hWinTail=hWinHead=pWin->hWnd;
				 memcpy(&(WinArray[i].win),pWin,sizeof(WINSTRUCT));
			   WinArray[i].bUsed=TRUE; 
			 }
			 else if(GetCurWinSolt(hWinTail)->winStyle!=eWinTaskBar)    //若未加入TASKBAR
			 {
			   pWin->hWinPrev=hWinTail;
	       pWin->hWinNext=NULL;
			   GetCurWinSolt(hWinTail)->hWinNext=pWin->hWnd;
			   hWinTail=pWin->hWnd;
			   memcpy(&(WinArray[i].win),pWin,sizeof(WINSTRUCT));
			   WinArray[i].bUsed=TRUE;
			 }
			 else
			 {
				 pWin->hWinPrev=GetCurWinSolt(hWinTail)->hWinPrev;
	       pWin->hWinNext=hWinTail;
			   GetCurWinSolt(GetCurWinSolt(hWinTail)->hWinPrev)->hWinNext=pWin->hWnd;
				 GetCurWinSolt(hWinTail)->hWinPrev=pWin->hWnd;
			   memcpy(&(WinArray[i].win),pWin,sizeof(WINSTRUCT));
			   WinArray[i].bUsed=TRUE;
			 }
			 
			 return TRUE;
		}
	return FALSE;
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
HDC GetDC(HWND hWnd)
{
	register int    i;
	HDC             hdc;

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			hdc=WinArray[i].win.winHDC;
		  return hdc;
	  }
	}
	return NULL;
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
RECT GetWinRect(HWND hWnd)
{
	register int    i;
	RECT            rc;

	rc=SetRect(0,0,0,0);

	for(i=0;i<MAX_WINSLOT;i++)
	{
		if(WinArray[i].bUsed && hWnd==WinArray[i].win.hWnd)
		{	  
			rc=WinArray[i].win.winRect;
		  return rc;
	  }
	}
	return rc;

}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
RECT   ScreenRectToDC(HDC hdc,RECT* sRect)
{
	RECT   rc;
	rc.left=sRect->left-hdc->Bounds.left;
	rc.top=sRect->top-hdc->Bounds.top;
	rc.right=sRect->right-hdc->Bounds.left;
	rc.bottom=sRect->bottom-hdc->Bounds.top;

	if(rc.left<0 && rc.right<0) {rc=SetRect(0,0,0,0);return rc;}
	if(rc.top<0 && rc.bottom<0) {rc=SetRect(0,0,0,0);return rc;}
	if(rc.left<=0) rc.left=0;
	if(rc.top<=0) rc.top=0;
	return rc;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
RECT  DCRectToScreen(HDC hdc,RECT* dRect)
{
   RECT   rc;
	 rc.left=hdc->Bounds.left+dRect->left;
	 rc.top=hdc->Bounds.top+dRect->top;
	 rc.right=hdc->Bounds.left+dRect->right;
   rc.bottom=hdc->Bounds.top+dRect->bottom;
	 return rc;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL IntersectRect(RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
{
    pdrc->left = (psrc1->left > psrc2->left) ?    psrc1->left : psrc2->left;
    pdrc->top  = (psrc1->top > psrc2->top) ?      psrc1->top  : psrc2->top;
    pdrc->right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
    pdrc->bottom = (psrc1->bottom < psrc2->bottom) ? psrc1->bottom : psrc2->bottom;

    if(pdrc->left >= pdrc->right || pdrc->top >= pdrc->bottom)
    return FALSE;

    return TRUE;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL InciseClipRect(RESIDUALRGN* ResRng,const RECT* pOldRect, const RECT* pNewRect)
{
	RECT* rc=&(ResRng->rc[0]);
	int nCount=ResRng->rcNum=0;


	if((pNewRect->top) > (pOldRect->top))
  {
            rc[nCount].left  =  pOldRect->left;
            rc[nCount].top   =  pOldRect->top;
            rc[nCount].right =  pOldRect->right;
            rc[nCount].bottom = pNewRect->top;
						nCount++;
  }

  if((pNewRect->bottom) < (pOldRect->bottom))
  {
            rc[nCount].top  =   pNewRect->bottom;
            rc[nCount].left   = pOldRect->left;
            rc[nCount].right =  pOldRect->right;
            rc[nCount].bottom = pOldRect->bottom;
            nCount++;
  }

  if((pNewRect->left) > (pOldRect->left))
  {
            rc[nCount].left  =  pOldRect->left;
            rc[nCount].top   =  pOldRect->top;
            rc[nCount].right =  pNewRect->left;
            rc[nCount].bottom = pOldRect->bottom;
            nCount++;
  }

  if((pNewRect->right) < (pOldRect->right))
  {
            rc[nCount].left  =  pNewRect->right;
            rc[nCount].top   =  pOldRect->top;
            rc[nCount].right =  pOldRect->right;
            rc[nCount].bottom = pOldRect->bottom;
            nCount++;
  }

  if (nCount == 0) return FALSE;

	ResRng->rcNum=nCount;

	return TRUE;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL FindClipRect(CLIPRGN* pRgn,HWND hWnd)
{
	int i;
	for(i=0;i<pRgn->ClipNum;i++)
	{
		if(pRgn->ClipArray[i].hWnd==hWnd)
		return TRUE;
	}

	return FALSE;
}



//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL AddClipRect(CLIPRGN* pRgn,CLIPRECT ClipRect)
{
	if(pRgn->ClipNum>=64) return FALSE;

	pRgn->ClipArray[pRgn->ClipNum]=ClipRect;
	pRgn->ClipNum++;
	return TRUE;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL EditClipRect(CLIPRGN* pRgn,HWND hWnd,CLIPRECT ClipRect)
{
	int i;
	for(i=0;i<pRgn->ClipNum;i++)
		if(pRgn->ClipArray[i].hWnd==hWnd)
		{
        pRgn->ClipArray[i]=ClipRect;
		}
	return TRUE;
}

//--------------------------------------------------------------------------
//   check3
//--------------------------------------------------------------------------
BOOL DelClipRect(CLIPRGN* pRgn,HWND hWnd)
{
	int  aPos,i;

	if(pRgn->ClipNum==0) return FALSE;

	for(i=0;i<pRgn->ClipNum;i++)
		if(pRgn->ClipArray[i].hWnd==hWnd) break;

	if(i==pRgn->ClipNum) return FALSE;
  aPos=i;

	for(i=aPos;i<pRgn->ClipNum-1;i++)
  pRgn->ClipArray[i]=pRgn->ClipArray[i+1];

	memset(&pRgn->ClipArray[i],0,sizeof(CLIPRECT));

	pRgn->ClipNum--;
	return TRUE;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
BOOL ClipRectLoop(RECTRGN* pInvRgn,RECT* pClipRect)
{
	RECT                       ISRect;                          //相交矩形
	RESIDUALRGN     ResRng;                        //残余矩形
	RECTRGN              tempRng;
	int     i,j;

	tempRng.rcNum=0;

	for(i=0;i<pInvRgn->rcNum;i++)
	if(IntersectRect(&ISRect,&(pInvRgn->rc[i]),pClipRect))
	{
		 InciseClipRect(&ResRng,&(pInvRgn->rc[i]),pClipRect);
   
     for(j=0;j<ResRng.rcNum;j++)
		 {
			    tempRng.rc[tempRng.rcNum]=ResRng.rc[j];
		      tempRng.rcNum++;
		 }

		 if(ResRng.rcNum<=0)                              //相交且不剩余则为全部遮挡区
		 {
		    pInvRgn->rc[i]=SetRect(0,0,0,0);
		 }
	}
	else 
	{
		 tempRng.rc[tempRng.rcNum]=pInvRgn->rc[i];
		 tempRng.rcNum++;
	}

	if(tempRng.rcNum!=0)
	{
	   memcpy(pInvRgn,&tempRng,sizeof(RECTRGN));
	}
	return TRUE;
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void InvalidRectToVram(HDC hdc,RECT* pInvRect)
{

}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void SendInvalidRect(HWND hWnd,RECT InvRect)
{									
	KnlMes mes;
	HINSTANCE hInstance;

	hInstance=0;
	mes.MessageType=MES_ERASE;
	mes.Param1=SETLONGWORD(InvRect.left,InvRect.top);
	mes.Param2=SETLONGWORD(InvRect.right-InvRect.left,InvRect.bottom-InvRect.top);
	mes.hWnd= (unsigned long)(hWnd);
	mes.hInstance=hInstance;
	KnlSendMessage(hWnd,mes);
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void SetAboveClipRgn(HWND hWnd)
{
	HWND              CurHWnd=hWnd;
	CLIPRECT         ClipRect;
	RECT                  winRect;
	HDC                   hdc;
	WINSTRUCT*   pWin=GetCurWinSolt(hWnd);

	if(CurHWnd==hWinTail) return;

  winRect=pWin->winRect;
  ClipRect.bExist=TRUE;
  
	while(CurHWnd!=hWinTail)
	{
		 CurHWnd=GetNextWinSolt(CurHWnd);
	   hdc=GetDC(CurHWnd);

		 if(!FindClipRect(&(pWin->winHDC->ecrgn),CurHWnd))
		 {
		     if(IntersectRect(&ClipRect.rc,&hdc->Bounds,&winRect))
				 {
					   ClipRect.hWnd=CurHWnd;
		         AddClipRect(&(pWin->winHDC->ecrgn),ClipRect);						 
				 }
		 }
		 else
		 {
			 if(IntersectRect(&ClipRect.rc,&hdc->Bounds,&winRect))
			 {
					ClipRect.hWnd=CurHWnd;
		      EditClipRect(&(pWin->winHDC->ecrgn),CurHWnd,ClipRect);
			 }
			 else
			 {
				  DelClipRect(&(pWin->winHDC->ecrgn),CurHWnd);				  
			 }
		 }

	}

}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void SetBelowClipRgn(HWND hWnd)
{
	HWND                      CurHWnd=hWnd;
	CLIPRECT                  ClipRect;
	RECT                      winRect;
	HDC                       hdc=GetDC(hWnd);
	

  winRect=hdc->Bounds;

  ClipRect.bExist=TRUE;
	ClipRect.hWnd=hWnd;

	while(CurHWnd!=hWinHead)
	{
     CurHWnd=GetPrevWinSolt(CurHWnd);
		 hdc=GetDC(CurHWnd);

		 if(!FindClipRect(&hdc->ecrgn,hWnd))
		 {
		     if(IntersectRect(&ClipRect.rc,&hdc->Bounds,&winRect))
		         AddClipRect(&hdc->ecrgn,ClipRect);
		 }
		 else
		 {
			 if(IntersectRect(&ClipRect.rc,&hdc->Bounds,&winRect))
		      EditClipRect(&hdc->ecrgn,hWnd,ClipRect);
			 else
				 DelClipRect(&hdc->ecrgn,hWnd);				  
		 }
	}

}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void HdcSetPixel(HDC hdc,int x,int y)
{
	int sw=hdc->Bounds.right-hdc->Bounds.left;
	int sh=hdc->Bounds.bottom-hdc->Bounds.top;

	if(x<0  ||  x>sw-1 ||  y<0 || y>sh-1) return;

	hdc->mBuf[y*sw+x]=hdc->penColor;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcDrawVline(HDC hdc,int x1,int x2,int y)
{
	register int i;
	for(i=x1;i<x2;i++)
	HdcSetPixel(hdc,i,y);
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcDrawHline(HDC hdc,int y1,int y2,int x)
{
	register int i;
	for(i=y1;i<y2;i++)
	HdcSetPixel(hdc,x,i);
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcSetBkColor(HDC hdc,unsigned long color)
{
	hdc->bkColor=color;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcSetBrushColor(HDC hdc,unsigned long color)
{
	hdc->brushColor=color;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcSetPenColor(HDC hdc,unsigned long color)
{
	hdc->penColor=color;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcSetTextColor(HDC hdc,unsigned long color)
{
	hdc->textColor=color;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void HdcFillRect(HDC hdc,int x,int y,int w,int h)
{
	int i;
	HdcSetPenColor(hdc,hdc->brushColor);
	for(i=y;i<y+h;i++)
	HdcDrawVline(hdc,x,x+w,i);
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
__inline RGB GetRGB(unsigned char r,unsigned char g,unsigned char b)
{
	register unsigned long RGBr;
	register unsigned long RGBg;
	register unsigned long RGBb;
	RGBr=(unsigned long)r;
	RGBr=RGBr<<16;
	RGBg=(unsigned long)g;
	RGBg=RGBg<<8;
	RGBb=(unsigned long)b;
  return (unsigned long)(RGBr | RGBg | RGBb);
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
unsigned char GetRGBr(RGB aRGB)
{
	aRGB = aRGB & 0xFF0000;
	aRGB = aRGB >> 16;
  return (unsigned char)aRGB;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
unsigned char GetRGBg(RGB aRGB)
{
	aRGB = aRGB & 0x00FF00;
	aRGB = aRGB >> 8;
  return (unsigned char)aRGB;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
unsigned char GetRGBb(RGB aRGB)
{
	aRGB = aRGB & 0x0000FF;
  return (unsigned char)aRGB;
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void DrawFadeColor(HDC hdc,int x,int y,int w,int h,RGB FromColor)
{
	 int i;
	 int scale;
	 unsigned char FR;
	 unsigned char FG;
	 unsigned char FB;

	 FR = GetRGBr(FromColor);
   FG = GetRGBg(FromColor);
   FB = GetRGBb(FromColor); 

	 scale=12*w/255;
	 for(i=0;i<w;i++)
	 {
	    HdcSetPenColor(hdc,GetRGB(FR+2*(i/scale),FG+2*(i/scale),FB+2*(i/scale)));
	    HdcDrawHline(hdc,y,y+h,x+i);
	 }
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void DrawWinTitleBar(HDC hdc,int x,int y,int w,int h,RGB aColor)
{

	RGB FromColor=aColor;
	DrawFadeColor(hdc,x,y,w,h,FromColor);

}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void DrawWinUpFram(HDC hdc,int x,int y,int w,int h)
{
	HdcSetPenColor(hdc,0xFFFFFF);
	HdcDrawHline(hdc,y,y+h,x);
	HdcDrawVline(hdc,x,x+w,y);
	HdcSetPenColor(hdc,0x000000);
	HdcDrawHline(hdc,y,y+h-1,x+w-1);
	HdcDrawVline(hdc,x,x+w-1,y+h-1);
}


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void SetBelowInvalidRng(HWND hWnd,RECT OldRect)
{
	RECT InvRect;
	RECT ISRect;
	RESIDUALRGN ResRgn;
  HDC  hdc;
	HWND CurHWnd=hWnd;
	int i;

	hdc=GetDC(hWnd);

	if(IntersectRect(&ISRect,&hdc->Bounds,&OldRect))
		  InciseClipRect(&ResRgn,&OldRect,&hdc->Bounds);
	else
	{
		ResRgn.rcNum=1;
		ResRgn.rc[0]=OldRect;
	}

	 
	while(CurHWnd!=hWinHead)
	{
		 CurHWnd=GetPrevWinSolt(CurHWnd);
		 for(i=0;i<ResRgn.rcNum;i++)
		 {
			   InvRect=ScreenRectToDC(GetDC(CurHWnd),&ResRgn.rc[i]);
		     SendInvalidRect(CurHWnd,InvRect);
		 }
	}

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void SetActiveWindow(HWND hWnd)
{
		WINSTRUCT*  pWin;
		KnlMes      mes;
   
		SetTopWindow(hWnd);

		pWin=GetCurWinSolt(hWinTail);

	  while(pWin->hWnd!=hWinHead)
	  {

			if(pWin->winStyle==eWinNormal && pWin->hWnd!=hWnd)
			{
				mes.MessageType=MES_WND_KILL_ACTIVE;
			  KnlSendMessage(pWin->hWnd,mes);
			}

			pWin=GetCurWinSolt(pWin->hWinPrev);
	  }


		pWin=GetCurWinSolt(hWnd);

		if(pWin->winStyle==eWinNormal)
		{
			 mes.MessageType=MES_WND_ACTIVE;
			 KnlSendMessage(pWin->hWnd,mes);
		}

}


#ifndef GUI_H
#define GUI_H

#include "CTYPE.h"
#include "Queue.h"



#define MAX_WINSLOT 32

typedef unsigned long RGB;


typedef enum{
	eWinDesktop,
	eWinTaskBar,
	eWinNormal,
}EnumWinStyle;

typedef enum{
	eWinButton,
	eWinEditBox,
	eWinLabel,
	eWinScrollBar,
	eWinFlatButton,
	eWinMenuButton,
}EnumWinChildStyle;

typedef int (* WNDPROC)(HWND, int, WPARAM, LPARAM);


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _RECT
{
    int left;
    int top;
    int right;
    int bottom;
}RECT;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _POINT
{
    int x;
    int y;
}POINT;


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _CLIPRECT
{
	  RECT rc;
		HWND hWnd;
		BOOL bExist;
}CLIPRECT;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _CLIPRGN
{
	  RECT rcBound;
		int  ClipNum;
		CLIPRECT ClipArray[64];
}CLIPRGN;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
struct tagDC{
  RECT Bounds;
	unsigned long brushColor;
	unsigned long penColor;
	unsigned long bkColor;
	unsigned long textColor;
	unsigned long* mBuf;
	BOOL          bTextTransparent;
	CLIPRGN  ecrgn;
};

typedef struct tagDC  DC;
typedef struct tagDC* HDC;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _RESIDUALRGN
{
	int rcNum;
	RECT rc[4];
}RESIDUALRGN;


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _RECTRGN
{
	int rcNum;
	RECT rc[64];
}RECTRGN;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _WinTag
{
	RECT                 winOldRect;
	RECT                 winRect;
	//BOOL                 bActive;
	EnumWinStyle         winStyle;
	HDC                  winHDC;
	HWND                 hWnd;
	HINSTANCE            hInstance;
	HWND                 hWinPrev;
	HWND                 hWinNext;
	KnlQueue*            MessageQueue;
	int (*WindowProc)    (HWND, int, WPARAM, LPARAM);
	char                 winTitle[32];

}WINSTRUCT;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
typedef struct _WINSLOT
{
	WINSTRUCT          win;
	BOOL               bUsed;
}WINSLOT;


extern  WINSLOT WinArray[MAX_WINSLOT];

extern __inline RECT  SetRect(int left,int top,int right,int bottom)
{
	RECT rc;
	rc.left=left;
	rc.top=top;
	rc.right=right;
	rc.bottom=bottom;
	return rc;
}


BOOL AddWinSoltToArray(WINSTRUCT* pWin);
HDC  GetDC(HWND hWnd);
BOOL IntersectRect(RECT* pdrc, const RECT* psrc1, const RECT* psrc2);
WINSTRUCT* KnlGetWndPtr(HWND hWnd);
int  KnlSendMessage(HWND hWnd,KnlMes mes);
BOOL SetWndSize(HWND hWnd,int w,int h);
BOOL SetWndPos(HWND hWnd,int x,int y);
RECT GetWinRect(HWND hWnd);
void SetBelowInvalidRng(HWND hWnd,RECT OldRect);
void SetBelowClipRgn(HWND hWnd);
void SetAboveClipRgn(HWND hWnd);
void HdcShowBmp(HDC hdc,unsigned int x , unsigned int y,unsigned long FileAddr);
void BroadCastMessage(KnlMes mes);
HWND GetTopMainWin(int x,int y);
BOOL SetTopWindow(HWND hWnd);
void SetActiveWindow(HWND hWnd);
WINSTRUCT*  GetCurWinSolt(HWND hWnd);

#endif

//---------------------------------------------------------------------------------------------------------------------------
//          Graphic Driver For FireXOS  1.10  (Graphic Driver For GUI Edition)
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by  xiaofan                                   2004.9.20
//---------------------------------------------------------------------------------------------------------------------------
#include "CTYPE.h"
#include "VBE.h"
#include "GraphicDriver.h"
#include "Stdarg.h"

static unsigned long KernelTextColor=0xFFFFFF;

static KernelGraphicDriver* KnlGraphDrvPtr=NULL;                //核心图形驱动指针

static char argBuf[1024];

//------------------------------------------------------------------------------------------------------------------------------
//
// 安装核心图形驱动
//
//------------------------------------------------------------------------------------------------------------------------------
void KernelInstallGraphicDriver(KernelGraphicDriver* GraphicDriverPtr)
{
     KnlGraphDrvPtr=GraphicDriverPtr;
}


//------------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动初始化
//
//------------------------------------------------------------------------------------------------------------------------------
void KernelGraphicInit(void)
{
	   VesaInstallDriver();
     KnlGraphDrvPtr->Init();
}


//------------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动画点例程
//
//------------------------------------------------------------------------------------------------------------------------------
void KernelPutPixel(unsigned short x,unsigned short y,unsigned long color)
{
	 KnlGraphDrvPtr->PutPixel(x,y,color);
}


//-----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动画水平线例程
//
//-----------------------------------------------------------------------------------------------------------------------------
void KernelDrawVline(unsigned short x1,unsigned short x2,unsigned short y, unsigned long color)
{
     KnlGraphDrvPtr->DrawVline(x1,x2,y,color);
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动画垂直线例程
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelDrawHline(unsigned short y1,unsigned short y2,unsigned short x, unsigned long color)
{
	 KnlGraphDrvPtr->DrawHline(y1,y2,x,color);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动清屏例程
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelClearBk(void)
{
	 KnlGraphDrvPtr->ClearBk();
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动填充矩形例程
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelFillRect(unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned long color)
{
	 KnlGraphDrvPtr->FillRect(x,y,w,h,color);
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动输出ASCII字符例程
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelOutAsc(unsigned short x, unsigned short y, unsigned char ch, unsigned long color)
{
	 KnlGraphDrvPtr->OutAsc(x,y,ch,color);
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动输出字符串例程
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelTextOut(unsigned short x,unsigned short y,char * str)
{
	 int i=0;
	 char *p=str;
	 while(*p!='\0')
	 {           
		KernelOutAsc(x+i,y,*p++,KernelTextColor);          
		i+=8;
	 }
}

//---------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动设定背景色例程
//
//---------------------------------------------------------------------------------------------------------------------------
void KernelSetBkColor(unsigned long color)
{
  KnlGraphDrvPtr->SetBkColor(color);
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动输出字符串例程
//
//----------------------------------------------------------------------------------------------------------------------------
int KernelPrintf(unsigned short x,unsigned short y,const char * fmt, ...)
{
	int i;

	va_list args;
	va_start(args, fmt);
	i = vsprintf(argBuf, fmt, args);
	va_end(args);

	KernelTextOut(x,y,argBuf);
	return i;
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动保存显存矩形区域到鼠标缓冲区
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelVramToMsBuf(RECT msRect,RECT sRect,unsigned long* buf)
{
     KnlGraphDrvPtr->VramToMsBuf(msRect,sRect,buf);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动保存显存矩形区域到缓冲区
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelVramToBuf(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf)
{
     KnlGraphDrvPtr->VramToBuf(x1,y1,x2,y2,buf);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 核心图形驱动缓冲区数据拷到显存区域
//
//----------------------------------------------------------------------------------------------------------------------------
void KernelBufToVram(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf)
{
     KnlGraphDrvPtr->BufToVram(x1,y1,x2,y2,buf);
}

//---------------------------------------------------------------------------------------------
//
// 核心图形驱动获得屏幕宽度
//
//---------------------------------------------------------------------------------------------
int  KernelGetScreenWidth(void)
{
     return KnlGraphDrvPtr->GetScreenWidth();
}

//---------------------------------------------------------------------------------------------
//
// 核心图形驱动获得屏幕高度
//
//---------------------------------------------------------------------------------------------
int  KernelGetScreenHeight(void)
{
     return KnlGraphDrvPtr->GetScreenHeight();
}


//---------------------------------------------------------------------------------------------
//
// 核心图形驱动屏幕向下滚屏 n 个像素行
//
//---------------------------------------------------------------------------------------------
void KernelScreenScrollDown(int n)
{
     KnlGraphDrvPtr->ScreenScrollDown(n);
}

//---------------------------------------------------------------------------------------------
//
// 核心图形驱动屏幕向上滚屏 n 个像素行
//
//---------------------------------------------------------------------------------------------
void KernelScreenScrollUp(int n)
{
     KnlGraphDrvPtr->ScreenScrollUp(n);
}


//---------------------------------------------------------------------------------------------
//
// 核心图形驱动设定字符颜色例程
//
//---------------------------------------------------------------------------------------------
void KernelSetTextColor(unsigned long color)
{
  KernelTextColor=color;
}

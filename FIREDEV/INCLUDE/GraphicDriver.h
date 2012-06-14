//---------------------------------------------------------------------------------------------------------------------------
//          Graphic Driver Header For FireXOS  1.01  (Graphic Driver For GUI Edition)
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//---------------------------------------------------------------------------------------------------------------------------

#ifndef GRAPHICDRIVER_H
#define GRAPHICDRIVER_H

#include "GUI.h"

//核心图形驱动结构体
typedef struct KernelGraphicDriver
{
  void (*Init)        (void);
  void (*PutPixel)    (unsigned short x,unsigned short y,unsigned long color);
  void (*DrawVline)   (unsigned short x1,unsigned short x2,unsigned short y, unsigned long color);
  void (*DrawHline)   (unsigned short y1,unsigned short y2,unsigned short x, unsigned long color);
  void (*ClearBk)     (void);
  void (*FillRect)    (unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned long color);
  void (*OutAsc)      (unsigned short x, unsigned short y, unsigned char ch, unsigned long color);
	void (*SetBkColor)  (unsigned long color);
	void (*VramToMsBuf) (RECT msRect,RECT sRect,unsigned long* buf);
	void (*VramToBuf)   (unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf);
	void (*BufToVram)   (unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf);
	int  (*GetScreenWidth)  (void);
	int  (*GetScreenHeight) (void);
	void (*ScreenScrollDown) (int n);
	void (*ScreenScrollUp)   (int n);
} KernelGraphicDriver;


//---------------------------------------------------------------------------------------------------------------------------------------------
// 核心图形驱动接口函数
//---------------------------------------------------------------------------------------------------------------------------------------------
void KernelInstallGraphicDriver(KernelGraphicDriver* GraphicDriverPtr);

void KernelGraphicInit(void);

void KernelPutPixel(unsigned short x,unsigned short y,unsigned long color);

void KernelDrawVline(unsigned short x1,unsigned short x2,unsigned short y, unsigned long color);

void KernelDrawHline(unsigned short y1,unsigned short y2,unsigned short x, unsigned long color);

void KernelClearBk(void);

void KernelFillRect(unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned long color);

void KernelOutAsc(unsigned short x, unsigned short y, unsigned char ch, unsigned long color);

void KernelTextOut(unsigned short x,unsigned short y,char * str);

void KernelScrollUp(int n);

void KernelSetBkColor(unsigned long color);

void KernelSetTextColor(unsigned long color);

int  KernelPrintf(unsigned short x,unsigned short y,const char * fmt, ...);

void KernelVramToMsBuf(RECT msRect,RECT sRect,unsigned long* buf);

void KernelVramToBuf(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf);

void KernelBufToVram(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf);

int  KernelGetScreenWidth(void);

int  KernelGetScreenHeight(void);

void KernelScreenScrollDown(int n);

void KernelScreenScrollUp(int n);



#endif

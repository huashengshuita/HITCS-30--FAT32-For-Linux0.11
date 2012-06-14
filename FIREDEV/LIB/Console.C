
//-----------------------------------------------------------------------------------------------------------------------
//          Kernel Console Lib For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "GraphicDriver.h"
#include "Stdarg.h"
#include "Schedule.h"

#define    CONSOLE_INIT_ROW  5
#define    CONSOLE_FONT_W    8
#define    CONSOLE_FONT_H    16

static int ConsoleMaxCol=0;
static int ConsoleMaxRow=0;

static int ConsoleCurRow=5;
static int ConsoleCurCol=0;

static unsigned long ConsoleBkColor=0;
static unsigned long ConsoleTextColor=0;


static char argBuf[1024];


//---------------------------------------------------------------------------------------------------------------------------
//
// 设置控制台文字颜色
//
//---------------------------------------------------------------------------------------------------------------------------
void ConsoleSetTextColor(unsigned long Color)
{
   ConsoleTextColor=Color;	 
}

//---------------------------------------------------------------------------------------------------------------------------
//
// 设置控制台背景颜色
//
//---------------------------------------------------------------------------------------------------------------------------
void ConsoleSetBkColor(unsigned long Color)
{
   ConsoleBkColor=Color;	 
}

//---------------------------------------------------------------------------------------------------------------------------
//
// 控制台向下滚屏 n 个像素行
//
//---------------------------------------------------------------------------------------------------------------------------
void ConsoleScrollDown(int nRow)
{
	 KernelScreenScrollDown(nRow*CONSOLE_FONT_H);
	 KernelFillRect(0,0,ConsoleMaxCol*CONSOLE_FONT_W,nRow*CONSOLE_FONT_H,ConsoleBkColor);
}

//---------------------------------------------------------------------------------------------------------------------------
//
// 控制台向上滚屏 n 个像素行
//
//---------------------------------------------------------------------------------------------------------------------------
void ConsoleScrollUp(int nRow)
{
	 KernelScreenScrollUp(nRow*CONSOLE_FONT_H);
	 KernelFillRect(0,(ConsoleMaxRow-nRow)*CONSOLE_FONT_H,
	 	              ConsoleMaxCol*CONSOLE_FONT_W,ConsoleMaxRow*CONSOLE_FONT_H,ConsoleBkColor);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 打印OK信息
//
//----------------------------------------------------------------------------------------------------------------------------

void PrintOK(unsigned short x,unsigned short y)
{
	KernelSetTextColor(0xFFFFFF);
	KernelTextOut(x,y,"[");
	KernelSetTextColor(0x00FF00);
	KernelTextOut(x+8,y,"OK");
	KernelSetTextColor(0xFFFFFF);
	KernelTextOut(x+24,y,"]");
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 打印Fault信息
//
//----------------------------------------------------------------------------------------------------------------------------

void PrintFault(unsigned short x,unsigned short y)
{
	KernelSetTextColor(0xFFFFFF);
	KernelTextOut(x,y,"[");
	KernelSetTextColor(0xFF0000);
	KernelTextOut(x+8,y,"Fault");
	KernelSetTextColor(0xFFFFFF);
	KernelTextOut(x+48,y,"]");
}


//---------------------------------------------------------------------------------------------------------------------------
//
// 控制台初始化
//
//---------------------------------------------------------------------------------------------------------------------------
void ConsoleInit(int BkColor,int TextColor)
{
	 ConsoleBkColor=BkColor;
	 ConsoleTextColor=TextColor;

	 PrintOK(800-64,80);

	 ConsoleMaxCol=KernelGetScreenWidth()/CONSOLE_FONT_W;
	 ConsoleMaxRow=KernelGetScreenHeight()/CONSOLE_FONT_H;
   
	 KernelFillRect(0,CONSOLE_INIT_ROW*CONSOLE_FONT_H,
	 	              ConsoleMaxCol*CONSOLE_FONT_W,ConsoleMaxRow*CONSOLE_FONT_H,ConsoleBkColor);


}

//----------------------------------------------------------------------------------------------------------------------------
//
// 控制台输出ASCII字符
//
//----------------------------------------------------------------------------------------------------------------------------
void ConsoleOutAsc(unsigned char ch)
{
	  if(ch!='\n')
		{
       KernelOutAsc(ConsoleCurCol*CONSOLE_FONT_W,ConsoleCurRow*CONSOLE_FONT_H,ch,ConsoleTextColor);
       ConsoleCurCol++;
			 if(ConsoleCurCol>ConsoleMaxCol-1)
		   {
			    ConsoleCurCol=0;
			    ConsoleCurRow++;
		   }
		}
		else
		{
			ConsoleCurCol=0;
			ConsoleCurRow++;
		}
			 

		if(ConsoleCurRow>ConsoleMaxRow-1)
		{
			ConsoleScrollUp(1);
			ConsoleCurRow--;
		}

}


//----------------------------------------------------------------------------------------------------------------------------
//
// 控制台输出信息
//
//----------------------------------------------------------------------------------------------------------------------------
int ConsoleMsg(const char * fmt, ...)
{
	int i;
	char *p;

	va_list args;
	va_start(args, fmt);
	i = vsprintf(argBuf, fmt, args);
	va_end(args);

  p=argBuf;

	ConsoleSetTextColor(0x00FFFF);

	while(*p!='\0')
	{           
		ConsoleOutAsc(*p);
		p++;
	}

	ConsoleSetTextColor(0xFFFFFF);

	return i;
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 控制台输出警告信息
//
//----------------------------------------------------------------------------------------------------------------------------
int ConsoleWarning(const char * fmt, ...)
{
	int i;
	char *p;

	va_list args;
	va_start(args, fmt);
	i = vsprintf(argBuf, fmt, args);
	va_end(args);

  p=argBuf;

	ConsoleSetTextColor(0xFF0000);

	while(*p!='\0')
	{           
		ConsoleOutAsc(*p);
		p++;
	}

	ConsoleSetTextColor(0xFFFFFF);

	return i;
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 控制台打印OK信息
//
//----------------------------------------------------------------------------------------------------------------------------
void ConsolePrintOK(void)
{
  PrintOK(ConsoleMaxCol*CONSOLE_FONT_W-64,ConsoleCurRow*CONSOLE_FONT_H);
	ConsoleOutAsc('\n');
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 控制台打印Fault信息
//
//----------------------------------------------------------------------------------------------------------------------------
void ConsolePrintFault(void)
{
	PrintFault(ConsoleMaxCol*CONSOLE_FONT_W-64,ConsoleCurRow*CONSOLE_FONT_H);
	ConsoleOutAsc('\n');
}


//---------------------------------------------------------------------------------------------------------------------------
//
// 控制台Printf
//
//---------------------------------------------------------------------------------------------------------------------------
int ConsolePrintf(const char * fmt, ...)
{
	int i;
	char *p;

	va_list args;
	va_start(args, fmt);
	i = vsprintf(argBuf, fmt, args);
	va_end(args);

  p=argBuf;

	//SetSwitching(1);

	while(*p!='\0')
	{           
		ConsoleOutAsc(*p);
		p++;
	}

	//SetSwitching(0);

	return i;
}

//---------------------------------------------------------------------------------------------------------------------------
//          VESA Driver For FireXOS  1.10  (VESA Driver For GUI Edition)
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                   2004.9.20
//---------------------------------------------------------------------------------------------------------------------------
#include "GRUB.h"
#include "GraphicDriver.h"
#include "VBE.h"
#include "Console.h"



void VesaInit(void);
void VesaPutPixel(unsigned short x,unsigned short y,unsigned long color);
void VesaDrawVline(unsigned short x1,unsigned short x2,unsigned short y, unsigned long color);
void VesaDrawHline(unsigned short y1,unsigned short y2,unsigned short x, unsigned long color);
void VesaClearBk(void);
void VesaFillRect(unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned long color);
void VesaOutAsc(unsigned short x, unsigned short y, unsigned char ch, unsigned long color);
void VesaSetBkColor(unsigned long color);
void VesaVramToMsBuf(RECT msRect,RECT sRect,unsigned long* buf);
void VesaVramToBuf(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf);
void VesaBufToVram(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf);
int  VesaGetScreenWidth(void);
int  VesaGetScreenHeight(void);
void VesaScreenScrollDown(int n);
void VesaScreenScrollUp(int n);

static KernelGraphicDriver VESAGraphicDriver ={
  VesaInit,
	VesaPutPixel,
	VesaDrawVline,
	VesaDrawHline,
	VesaClearBk,
	VesaFillRect,
	VesaOutAsc,
	VesaSetBkColor,
	VesaVramToMsBuf,
	VesaVramToBuf,
  VesaBufToVram,
	VesaGetScreenWidth,
  VesaGetScreenHeight,
	VesaScreenScrollDown,
	VesaScreenScrollUp,
};

unsigned short  VESA_X_MAX;
unsigned short  VESA_Y_MAX;
unsigned long   *VGAMEM;
unsigned long   BG_COLOR=0x000000;

#define ASC_WIDTH                      8
#define CHS_WIDTH                      16
#define SPACING                        2
#define CHAR_HEIGHT                    16

#define ASC_NUM 256
extern unsigned char ASC[ASC_NUM][CHAR_HEIGHT];

extern GrubInfo* GrubBootInfo;

//------------------------------------------------------------------------------------------------------------------------------
//
// 安装VESA图形驱动
//
//------------------------------------------------------------------------------------------------------------------------------
void VesaInstallDriver(void)
{
	 KernelInstallGraphicDriver(&VESAGraphicDriver);
}


//------------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动初始化
//
//------------------------------------------------------------------------------------------------------------------------------
void VesaInit(void)
{

	struct VBEMode          *mode_info = (struct VBEMode*)       GrubBootInfo->vbe_mode_info;
  //struct VBEController    *control=    (struct VBEController*) GrubBootInfo->vbe_control_info;

	VESA_X_MAX = mode_info->x_resolution;
	VESA_Y_MAX = mode_info->y_resolution;

	VGAMEM = (unsigned long *) 0xFFC00000;

}

//------------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动画点例程
//
//------------------------------------------------------------------------------------------------------------------------------
void VesaPutPixel(unsigned short x,unsigned short y,unsigned long color)
{
	if(x<VESA_X_MAX && y<VESA_Y_MAX)
	VGAMEM[y*VESA_X_MAX+x]=color;
}

//-----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动画水平线例程
//
//-----------------------------------------------------------------------------------------------------------------------------
void VesaDrawVline(unsigned short x1,unsigned short x2,unsigned short y, unsigned long color)
{
	register int i;
	unsigned long StartPos,EndPos;

	StartPos=y*VESA_X_MAX+x1;
  EndPos=y*VESA_X_MAX+x2;

	for(i=StartPos;i<EndPos;i++)
  VGAMEM[i]=color;
}

//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动画垂直线例程
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaDrawHline(unsigned short y1,unsigned short y2,unsigned short x, unsigned long color)
{
	register int i;
	for(i=y1;i<y2;i++)
	VesaPutPixel(x,i,color);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动清屏例程
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaClearBk(void)
{
	unsigned long  ScreenSize,i;
	ScreenSize = VESA_X_MAX * VESA_Y_MAX;
  for(i = 0; i < ScreenSize; i++) 
	VGAMEM[i] = BG_COLOR;
}


//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动填充矩形例程
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaFillRect(unsigned short x,unsigned short y,unsigned short w,
                               unsigned short h,unsigned long color)
{
	register int i;
	for(i=y;i<y+h;i++)
	VesaDrawVline(x,x+w,i,color);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动输出ASCII字符例程
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaOutAsc(unsigned short x, unsigned short y, unsigned char ch, unsigned long color)
{
    register int i,j;
    for (i = 0; i < CHAR_HEIGHT; i++)
	  for (j=0;j<ASC_WIDTH;j++)
	  if((ASC[ch][i]>>(ASC_WIDTH-j-1))&1)
	  VesaPutPixel(x+j,y+i,color);
}

//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动保存显存矩形区域到鼠标缓冲区
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaVramToMsBuf(RECT msRect,RECT sRect,unsigned long* buf)
{
	register int i,j;
	int      w,h;

	h=sRect.bottom-sRect.top;
	w=sRect.right-sRect.left;

	for(j=0;j<h;j++)
	for(i=0;i<w;i++)
				buf[(j+sRect.top-msRect.top)*(msRect.right-msRect.left)+i+sRect.left-msRect.left]=
				VGAMEM[(sRect.top+j)*VESA_X_MAX+sRect.left+i];

}



//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动保存显存矩形区域到缓冲区
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaVramToBuf(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf)
{
	register int i,j;
	for(j=0;j<y2-y1;j++)
		for(i=0;i<x2-x1;i++)
			  if(x1+i>=0 && x1+i<VESA_X_MAX && y1+j>=0 && y1+j<VESA_Y_MAX)
				buf[j*(x2-x1)+i]=VGAMEM[(y1+j)*VESA_X_MAX+x1+i];
}


//----------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动保存显存矩形区域到缓冲区
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaBufToVram(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long* buf)
{
	register int i,j;
	for(j=0;j<y2-y1;j++)
		for(i=0;i<x2-x1;i++)
				VGAMEM[(y1+j)*VESA_X_MAX+x1+i]=buf[j*(x2-x1)+i];                             
}


//---------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动设定背景色例程
//
//---------------------------------------------------------------------------------------------------------------------------
void VesaSetBkColor(unsigned long color)
{
	BG_COLOR=color;
}

//---------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动获得屏幕宽度
//
//---------------------------------------------------------------------------------------------------------------------------
int VesaGetScreenWidth(void)
{
	  return VESA_X_MAX;
}


//---------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动获得屏幕高度
//
//---------------------------------------------------------------------------------------------------------------------------
int VesaGetScreenHeight(void)
{
	  return VESA_Y_MAX;
}

//---------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动向下滚屏 n 个像素行
//
//---------------------------------------------------------------------------------------------------------------------------
void VesaScreenScrollDown(int n)
{
	  //unsigned char* ToStart=(unsigned char*)&VGAMEM[VESA_X_MAX*n-1];
		unsigned char* ToEnd=(unsigned char*)&VGAMEM[VESA_X_MAX*VESA_Y_MAX-1];
		unsigned char* FromStart=(unsigned char*)&VGAMEM[0];
		unsigned char* FromEnd=(unsigned char*)&VGAMEM[VESA_X_MAX*(VESA_Y_MAX-n)-1];
    register unsigned char* p1;
		register unsigned char* p2;
    
	  for(p1=FromEnd,p2=ToEnd;p1>FromStart;p1--)
		{
		   	*p2=*p1;
				 p2--;
		}

}

//---------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动向上滚屏 n 个像素行
//
//---------------------------------------------------------------------------------------------------------------------------
void VesaScreenScrollUp(int n)
{
	  unsigned char* ToStart=(unsigned char*)&VGAMEM[0];
		//unsigned char* ToEnd=(unsigned char*)&VGAMEM[VESA_X_MAX*(VESA_Y_MAX-n)-1];
		unsigned char* FromStart=(unsigned char*)&VGAMEM[VESA_X_MAX*n];
		unsigned char* FromEnd=(unsigned char*)&VGAMEM[VESA_X_MAX*VESA_Y_MAX-1];
    register unsigned char* p1;
		register unsigned char* p2;
    
	  for(p1=FromStart,p2=ToStart;p1<FromEnd;p1++)
		{
		   	*p2=*p1;
				 p2++;
		}

}






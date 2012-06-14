#include "GRUB.h"
#include "VBE.h"


static unsigned int  LFBWidth;
static unsigned int  LFBHeight;
static unsigned int  LFBDepth;
static unsigned int  LFBType;
static unsigned long LFBPtr;
static unsigned int  LFBScanline;


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


//------------------------------------------------------------------------------------------------------------------------------
//
// VESA图形驱动画点例程
//
//------------------------------------------------------------------------------------------------------------------------------
void VesaPutPixel(unsigned short x,unsigned short y,unsigned long color)
{
	VGAMEM[y*VESA_X_MAX+x]=color;
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
// 核心图形驱动输出字符串例程
//
//----------------------------------------------------------------------------------------------------------------------------
void VesaTextOut(unsigned short x,unsigned short y,char * str,unsigned long color)
{
	 int i=0;
	 char *p=str;
	 while(*p!='\0')
	 {           
		VesaOutAsc(x+i,y,*p++,color);          
		i+=8;
	 }
}


//----------------------------------------------------------------------------------------------------------------------------
//
// VESA显卡初始化
//
//----------------------------------------------------------------------------------------------------------------------------
int LFBInit(GrubInfo* info)
{
	  unsigned long  *LFBAddr = (unsigned long *) 0x80228;
	  unsigned long i,ScreenSize;

	  struct VBEMode          *mode_info = (struct VBEMode*)       info->vbe_mode_info;
    struct VBEController    *control=    (struct VBEController*) info->vbe_control_info;

    if (control->version < 0x0200)
    {
      return 0;
    }

   if (CHECK_GRUB(info->flags,11)){
	    LFBWidth  =  mode_info->x_resolution;
	    LFBHeight =  mode_info->y_resolution;
	    LFBDepth  =  mode_info->bits_per_pixel;
	    LFBType   =  0;

			VESA_X_MAX=LFBWidth;
			VESA_Y_MAX=LFBHeight;

        if(control->version >= 0x0300)
                LFBScanline = mode_info->linear_bytes_per_scanline;
		else
		            LFBScanline = mode_info->bytes_per_scanline;

	    LFBPtr = mode_info->phys_base;

			*LFBAddr=LFBPtr;

			VGAMEM = (unsigned long *)LFBPtr;
  
			ScreenSize = VESA_X_MAX * VESA_Y_MAX;
	    for(i = 0; i < ScreenSize; i++) VGAMEM[i] = BG_COLOR;
        
    
   }
   else	{
	    LFBWidth  =  LFBHeight = LFBDepth = 0;
		  LFBType   = 0;
	    LFBPtr    = 0;
		  return 0;
	}

	 return 1;

}

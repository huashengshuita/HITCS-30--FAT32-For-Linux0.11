//-----------------------------------------------------------------------------------------------------------------------
//          Floppy Driver For FireXOS  1.01 
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                   2004.9.20
//------------------------------------------------------------------------------------------------------------------------
#include "String.h"
#include "CTYPE.h"
#include "Io.h"
#include "DMA.h"
#include "Traps.h"
#include "Floppy.h"
#include "Console.h"
#include "Timer.h"
#include "Stdarg.h"
#include "GraphicDriver.h"


#define ERROR_NO_FLOPPY    5

#define IoWait()   inb(0x80)

struct FloppyStruct
{
	char type[8];
	unsigned char  tracks;
	unsigned char  heads;
	unsigned char  sectors;

	unsigned char   request_sector;
	unsigned char*  buffer;
} FloppyDev;                                                              //软驱结构体

static unsigned char FloppyCashAddr[512];                                 //软盘高速缓冲区地址指针

#define MAX_REPLIES 7                                                             
static unsigned char FloppyReplyBuffer[MAX_REPLIES];                      //软驱回应缓冲区
#define ST0 (FloppyReplyBuffer[0])                                        //软驱回应0号字节
#define ST1 (FloppyReplyBuffer[1])                                        //软驱回应1号字节
#define ST2 (FloppyReplyBuffer[2])                                        //软驱回应2号字节
#define ST3 (FloppyReplyBuffer[3])                                        //软驱回应3号字节


static char FloppyIncName[24];                                            //软驱型号名

static int FloppyIntStatus=0;                                             //软驱中断状态字节

static int  FloppyMotor=0;                                                //软驱马达状态字节

void FloppyInterrupt(void);                                               //软驱中断句柄


int    InrType=0;

//-------------------------------------------------------------------------------------------------------------------------------
//
//  延迟函数
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyDelay(void)
{
	kernelDelay();
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  获得软驱响应状态
//
//-------------------------------------------------------------------------------------------------------------------------------
static int FloppyResult(void)
{
	unsigned char stat, i,count;
	i=0;
	for(count=0;count<10000;count++){
		stat = inb( FD_STATUS ) & (STATUS_READY|STATUS_DIR|STATUS_BUSY); //读取状态寄存器
		if (stat == STATUS_READY)
			return i;
		if (stat == (STATUS_READY|STATUS_DIR|STATUS_BUSY))
		{
			if(i>7) break;
			FloppyReplyBuffer[i++]=inb_p(FD_DATA);
		}
	}

	ConsoleWarning("Getstatus times out !\n");
	return -1;
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  软驱等待中断完成
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyWaitInt(void)
{

	for(;;)
	{
		if(FloppyIntStatus==1) {FloppyIntStatus=0;break;}
	}

	FloppyIntStatus=0;

	return;
}


//-------------------------------------------------------------------------------------------------------------------------------
//
//  软驱中断处理函数
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyHandle(void)
{
	FloppyIntStatus=1;
}


//-------------------------------------------------------------------------------------------------------------------------------
//
//  向软驱控制寄存器发送一个控制字节
//
//-------------------------------------------------------------------------------------------------------------------------------
int FloppySendByte( int value )
{
	unsigned char stat, i;

	for ( i = 0; i < 128; i++ ) {
		stat = inb( FD_STATUS ) & (STATUS_READY|STATUS_DIR); //读取状态寄存器
		if  ( stat  == STATUS_READY )
		{
			outb( value ,FD_DATA);                             //将参数写入数据寄存器
			return 1;
		}
		FloppyDelay();                                       // 作一些延迟
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  从软驱数据寄存器得到一个数据字节
//
//-------------------------------------------------------------------------------------------------------------------------------
int FloppyGetByte(void)
{
	unsigned char stat, i;

	for ( i = 0; i < 128; i++ ) {
		stat = inb( FD_STATUS ) & (STATUS_READY|STATUS_DIR|STATUS_BUSY); //读取状态寄存器
		if (stat == STATUS_READY)
			return -1;
		if ( stat  == 0xD0 )
			return inb(FD_DATA);                    //获取数据
		FloppyDelay();                            // 作一些延迟
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  得到软驱信息
//
//-------------------------------------------------------------------------------------------------------------------------------
int GetFloppyInfo(void)
{
	unsigned int i;
	unsigned char CmType, FdType;

	FloppySendByte(0x10);
	i = FloppyGetByte(); 

	switch (i)
	{
	           case 0x80:	  strcpy(FloppyIncName,"NEC765A controller"); break;
	           case 0x90:   strcpy(FloppyIncName,"NEC765B controller"); break;
	           default:     strcpy(FloppyIncName,"Enhanced controller"); break;
	}

	CmType = ReadCmos(0x10);     //read floppy type from cmos
	FdType   = (CmType>>4) & 0x07;

	if ( FdType == 0 )  return ERROR_NO_FLOPPY;

	switch( FdType )
	{
	case 0x02: // 1.2MB
		strcpy(FloppyDev.type,"1.2MB");
	break;

	case 0x04: // 1.44MB       标准软盘
		strcpy(FloppyDev.type,"1.44MB");
		break;

	case 0x05: // 2.88MB
		strcpy(FloppyDev.type,"2.88MB");
		break;
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  软驱驱动初始化
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyInit(void)
{
	ConsolePrintf("Set the floppy driver");
	if(!GetFloppyInfo()) {return;};

	SetIntrGate(0x26,(void*)FloppyInterrupt);
	outb_p(inb_p(0x21)&~0x40,0x21);

	ConsolePrintOK();

	ConsoleMsg("\n");
	ConsoleMsg("  Floppy Inc : %s\n  Floppy Type : %s\n",FloppyIncName,FloppyDev.type);
	ConsoleMsg("\n");

	TickDelay(1);
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  打开软驱马达
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyMotorOn( void )
{
	if (!FloppyMotor)
	{
		__asm__("cli");
		outb(28,FD_DOR);
		FloppyMotor = 1;
		__asm__("sti");
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  关闭软驱马达
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyMotorOff( void )
{
	if (FloppyMotor)
	{
		__asm__("cli");
		outb(12,FD_DOR);
		FloppyMotor = 0;
		__asm__("sti");

	}
	return;
}


//-------------------------------------------------------------------------------------------------------------------------------
//
//  软驱寻道函数
//
//-------------------------------------------------------------------------------------------------------------------------------
int FloppySeek(int driver,int track,int head)
{
	int r1,r2;

	FloppySendByte( FD_SEEK);                                 //seek command
	FloppySendByte( head*4 + driver);
	FloppySendByte( track );
	FloppyWaitInt();

	
	FloppySendByte(FD_SENSEI);
	r1 = FloppyGetByte();
	r2 = FloppyGetByte();

	//ConsoleMsg("  Seek r1=%d r2=%d\n",r1,r2);
	/*  r1 is status register 0  */
	if ((r1 & 0xf8)==0x20 && r2==track)    {return 1;}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------------
//
//  软驱模式设置
//
//-------------------------------------------------------------------------------------------------------------------------------
static void FloppySetMode(void)
{   
	FloppySendByte (FD_SPECIFY);
	FloppySendByte (0xcf);
	FloppySendByte (0x06);
	outb (0,FD_DCR);
}


//-------------------------------------------------------------------------------------------------------------------------------
//
//  逻辑块转为磁盘头、磁道号和扇区号
//
//-------------------------------------------------------------------------------------------------------------------------------
void BlockToHTS(unsigned long block, int *head, int *track, int *sector )
{
	*head = ( block % ( 18 * 2 ) ) /18;
	*track =  block / ( 18 * 2 );
	*sector = block % 18 + 1;
}

//-------------------------------------------------------------------------------------------------------------------------------
//
//  软驱重置
//
//-------------------------------------------------------------------------------------------------------------------------------
/*
static void RecalibrateFloppy(void)
{
	FloppySendByte(FD_RECALIBRATE); //send recalibrate command 
	FloppySendByte(0);              // select driver A 
}
*/




//-------------------------------------------------------------------------------------------------------------------------------
//
//  设置软驱DMA通道
//
//-------------------------------------------------------------------------------------------------------------------------------
static void SetupFloppyDMA(void)
{  
	__asm__("cli");
	DisableDma(2);
	ClearDmaFF(2);
	SetDmaMode(2,DMA_MODE_READ);
	SetDmaAddr(2,(unsigned long)FloppyCashAddr);
	SetDmaCount(2,512);
	EnableDma(2);
	__asm__("sti");
}


//-------------------------------------------------------------------------------------------------------------------------------
//
//  从软盘上读取指定的逻辑块到缓冲区
//
//-------------------------------------------------------------------------------------------------------------------------------
void FloppyRead(int blk,int secs,void* buf)
{
	int head;
	int track;
	int sector;
	int res;


	BlockToHTS(blk,&head,&track,&sector);


	while(1)
	{

	FloppyMotorOn();
	FloppyDelay();                         // 作一些延迟
	__asm__("sti");

	//FloppyDelay();    // 作一些延迟
	//if(!FloppySeek(0,track,head)) continue;
  //FloppyDelay();    // 作一些延迟

	SetupFloppyDMA();
	FloppyDelay();    // 作一些延迟

	FloppySetMode();
	FloppyDelay();    // 作一些延迟
	FloppySendByte (FD_READ);              //send read command
	FloppySendByte (head*4 + 0);
	FloppySendByte (track);		             /*  Cylinder  */
	FloppySendByte (head);		             /*  Head  */
	FloppySendByte (sector);		           /*  Sector  */
	FloppySendByte (2);		                 /*  0=128, 1=256, 2=512, 3=1024, ...  */
	FloppySendByte (18);
	//FloppySendByte (sector+secs-1);	       /*  Last sector in track:here are  sectors count */
	FloppySendByte (0x1B);
	FloppySendByte (0xff);

	FloppyWaitInt();                       //直到发生了软驱中断


	res=FloppyResult();
  FloppyDelay();    // 作一些延迟

  if(ST1==0 && ST2 ==0)
	{
		 FloppyMotorOff();
     FloppyDelay();    // 作一些延迟
     memcpy(buf,(void*)FloppyCashAddr,secs*512);
		 return;
	}
	ConsoleWarning("  ST0 %d ST1 %d ST2 %d\n",ST0,ST1,ST2);
	}

  return;

}

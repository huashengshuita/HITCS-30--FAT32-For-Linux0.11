//-----------------------------------------------------------------------------------------------------------------------
//          PS/2 Mouse Driver For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "Io.h"
#include "GUI.h"
#include "CTYPE.h"
#include "Mouse.h"
#include "Message.h"
#include "PageAlloc.h"
#include "Queue.h"
#include "Traps.h"
#include "Schedule.h"
#include "Timer.h"
#include "Console.h"
#include "GraphicDriver.h"



void MouseInterrupt(void);


static                 RECT MouseRect;
static unsigned long   *MouseCursorBuf;
static int             mouse_old_x=0,mouse_old_y=0;


unsigned char ms_cursor_arrow[12*21] ={
	  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01,0x01, 0x01, 0x01, 0x01, 
		0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01,0x01, 0x01, 0x01, 0x01,
		0x00, 0x0F, 0x00, 0x01, 0x01, 0x01, 0x01,  0x01,0x01, 0x01, 0x01, 0x01, 
		0x00, 0x0F, 0x0F, 0x00, 0x01, 0x01, 0x01,  0x01,0x01, 0x01, 0x01, 0x01,
		0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x01, 0x01,  0x01,0x01, 0x01, 0x01, 0x01,
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x01,  0x01,0x01, 0x01, 0x01, 0x01,
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00,  0x01,0x01, 0x01, 0x01, 0x01, 
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  0x00,0x01, 0x01, 0x01, 0x01,  
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  0x0F,0x00, 0x01, 0x01, 0x01, 
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  0x0F,0x0F, 0x00, 0x01, 0x01, 
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  0x0F,0x0F, 0x0F, 0x00, 0x01, 
		0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  0x00,0x00, 0x00, 0x00, 0x00, 
		0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0x0F,  0x00,0x01, 0x01, 0x01, 0x01, 
		0x00, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F,  0x00,0x01, 0x01, 0x01, 0x01, 
		0x00, 0x0F, 0x00, 0x01, 0x01, 0x00, 0x0F,  0x0F,0x00, 0x01, 0x01, 0x01, 
		0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x0F,  0x0F,0x00, 0x01, 0x01, 0x01,
		0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x0F,0x0F, 0x00, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x0F,0x0F, 0x00, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x00,0x0F, 0x0F, 0x00, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x00,0x0F, 0x0F, 0x00, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01,0x00, 0x00, 0x01, 0x01,
};


//---------------------------------------------------------------------
//
//
//        DrawMouseArrow
//
//
//---------------------------------------------------------------------
void DrawMouseArrow(int x,int y)
{
	register int i,j;
	for(j=0;j<21;j++)
	for(i=0;i<12;i++)
	if(ms_cursor_arrow[j*12+i]==0x00)
		KernelPutPixel(x+i,y+j,0x000000);
	else if(ms_cursor_arrow[j*12+i]==0x0F)
		KernelPutPixel(x+i,y+j,0xFFFFFF);
}


//---------------------------------------------------------------------
//
//
//        InitMouseBuf
//
//
//---------------------------------------------------------------------
void InitMouseBuf(void)
{
	 RECT ISRect;
	 MouseCursorBuf=(unsigned long *)GetFreePage();
	 ISRect=MouseRect=SetRect(0,0,12,21);
	 KernelVramToMsBuf(MouseRect,ISRect,MouseCursorBuf);
	 DrawMouseArrow(0,0);
}

//---------------------------------------------------------------------
//
//
//        RefreshMouseBuf
//
//
//---------------------------------------------------------------------
void RefreshMouseBuf(RECT aInvRect)
{
	 RECT ISRect;
	 if(IntersectRect(&ISRect,&aInvRect,&MouseRect))
	   KernelVramToMsBuf(MouseRect,ISRect,MouseCursorBuf);
	 DrawMouseArrow(MouseRect.left,MouseRect.top);
}

//---------------------------------------------------------------------
//
//
//        SetMousePos
//
//
//---------------------------------------------------------------------
void SetMousePos(int *xChange,int *yChange)
{
	int mouse_x,mouse_y;
	RECT ISRect;

	mouse_x=MouseRect.left;
	mouse_y=MouseRect.top;

	if(mouse_x+(*xChange) >=0 && mouse_x+(*xChange)<800)   							      
	mouse_x+=(*xChange);
  else (*xChange)=0;

	if(mouse_y+(*yChange) >=0 && mouse_y+(*yChange)<600)
  mouse_y+=(*yChange);    
	else (*yChange)=0;


	SetSwitching(1);
	KernelBufToVram(mouse_old_x,mouse_old_y,mouse_old_x+12,mouse_old_y+21,MouseCursorBuf);
	ISRect=MouseRect=SetRect(mouse_x,mouse_y,mouse_x+12,mouse_y+21);
	KernelVramToMsBuf(MouseRect,ISRect,MouseCursorBuf);
	mouse_old_x=mouse_x;mouse_old_y=mouse_y;
	DrawMouseArrow(mouse_x,mouse_y);
	SetSwitching(0);
}


int GetMousePosX(void)
{
	return MouseRect.left;
}

int GetMousePosY(void)
{
	return MouseRect.top;
}


//---------------------------------------------------------------------
//
//
//        inPort60
//
//
//---------------------------------------------------------------------
static inline unsigned inPort60(void)
{
  // Input a value from the keyboard controller's data port, after checking
  // to make sure that there's some data there for us

  unsigned char data = 0;

  while (!(data & 0x01))
  data=inb(0x64);

  data=inb(0x60);
  return ((unsigned) data);
}


//---------------------------------------------------------------------
//
//
//        outPort60
//
//
//---------------------------------------------------------------------
static inline void outPort60(unsigned value)
{
  // Output a value to the keyboard controller's data port, after checking
  // to make sure it's ready for the data

  unsigned char data;
  
  // Wait for the controller to be ready
  data = 0x02;
  while (data & 0x02)
  data=inb(0x64);
  
  data = (unsigned char) value;
  outb(data,0x60);
  return;
}


//---------------------------------------------------------------------
//
//
//        outPort64
//
//
//---------------------------------------------------------------------
static inline void outPort64(unsigned value)
{
  // Output a value to the keyboard controller's command port, after checking
  // to make sure it's ready for the command

  unsigned char data;
  
  // Wait for the controller to be ready
  data = 0x02;
  while (data & 0x02)
  data=inb(0x64);

  data = (unsigned char) value;
  outb(data,0x64);

  return;
}


//---------------------------------------------------------------------
//
//
//        MouseHandle
//
//
//---------------------------------------------------------------------
void MouseHandle(void)
{
	static volatile int button1, button2, button3;
  unsigned char byte1, byte2, byte3;
	int           xChange, yChange;
  KnlMes   mes;


  // The first byte contains button information and sign information
  // for the next two bytes
  byte1 = inPort60();

  // The change in X position
  byte2 = inPort60();

  // The change in Y position
  byte3 = inPort60();
  
  
  if ((byte1 & 0x01))
  {
          if(button1 == 0)
          {
          mes.MessageType = MES_MOUSE_LDOWN;
					mes.Param1=SETLONGWORD(MouseRect.left,MouseRect.top);
	        KnlPutMessage(KnlMesQue,mes);
          button1=1;
          return;
          }
   }
  else if(button1 == 1)     //若此时左键已被按下,且0位不为1,则左键被弹起
  {
          mes.MessageType = MES_MOUSE_LUP;
					//mes.Param1=SETLONGWORD(MouseRect.left,MouseRect.top);
          KnlPutMessage(KnlMesQue,mes);
          button1=0;
          return;
  }

   if ((byte1 & 0x04))
  {
          if(button2 == 0)
          {
          mes.MessageType = MES_MOUSE_MDOWN;
					//mes.Param1=SETLONGWORD(MouseRect.left,MouseRect.top);
	        KnlPutMessage(KnlMesQue,mes);
          button2=1;
          return;
          }
   }
  else if(button2 == 1)     
  {
          mes.MessageType = MES_MOUSE_MUP;
					//mes.Param1=SETLONGWORD(MouseRect.left,MouseRect.top);
          KnlPutMessage(KnlMesQue,mes);
          button2=0;
          return;
  }

     if ((byte1 & 0x02))
  {
          if(button3 == 0)
          {
          mes.MessageType = MES_MOUSE_RDOWN;
					mes.Param1=SETLONGWORD(MouseRect.left,MouseRect.top);
	        KnlPutMessage(KnlMesQue,mes);
          button3=1;
          return;
          }
   }
  else if(button3 == 1)     
  {
          mes.MessageType = MES_MOUSE_RUP;
					//mes.Param1=SETLONGWORD(MouseRect.left,MouseRect.top);
          KnlPutMessage(KnlMesQue,mes);
          button3=0;
          return;
  }

      if (byte1 & 0x10)
	  xChange = (int) ((256 - byte2) * -1);
      else
	  xChange = (int) byte2;

      if (byte1 & 0x20)
	  yChange = (int) (256 - byte3);
      else
	  yChange = (int) (byte3 * -1);

      if(yChange !=0 && xChange !=0)
      {
         mes.MessageType = MES_MOUSE_MOVE;
         mes.Param1=xChange;
         mes.Param2=yChange;
         KnlPutMessage(KnlMesQue,mes);
      }
			
  return;
}

//---------------------------------------------------------------------
//
//
//        init the ps2 mouse
//
//
//---------------------------------------------------------------------
void MouseInit(void)
{
  unsigned char response;
  unsigned char deviceId;
  int count;

  ConsolePrintf("Set the mouse driver");

  for (count = 0; count < 10; count ++)
    {
      // Disable the mouse line
      outPort64(0xA7);
      
      // Enable the mouse line
      outPort64(0xA8);
      
      // Disable data reporting
      outPort64(0xD4);
      outPort60(0xF5);

      // Read the ack
      response = inPort60();
      if (response != 0xFA)
	continue;

      // Send reset command
      outPort64(0xD4);
      outPort60(0xFF);

      // Read the ack 0xFA
      response = inPort60();
      if (response != 0xFA)
	continue;

      // Should be 'self test passed' 0xAA
      response = inPort60();
      if (response != 0xAA)
	continue;

      // Get the device ID.  0x00 for normal PS/2 mouse
      deviceId = inPort60();

      // Set scaling to 2:1
      outPort64(0xD4);
      outPort60(0xE7);

      // Read the ack
      response = inPort60();
      if (response != 0xFA)
	continue;

      // Tell the controller to issue mouse interrupts
      outPort64(0x20);
      response = inPort60();
      response |= 0x02;
      outPort64(0x60);
      outPort60(response);

      // Enable data reporting (stream mode)
      outPort64(0xD4);
      outPort60(0xF4);

      // Read the ack
      response = inPort60();
      if (response != 0xFA)
	continue;

      // All set
      break;
    }
    SetIntrGate(0x2c,(void*)MouseInterrupt);
    outb_p(inb_p(0x21)&0xfb,0x21);             
    outb(inb_p(0xA1)&0xef,0xA1);   

		ConsolePrintOK();
		TickDelay(1);
}





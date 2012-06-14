//-----------------------------------------------------------------------------------------------------------------------
//          KeyBoard Driver For FireXOS  1.01 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "Keyboard.h"
#include "Queue.h"
#include "Message.h"
#include "Io.h"
#include "Traps.h"
#include "Timer.h"
#include "Console.h"

void  KeyboardInterrupt(void);                      //键盘中断句柄


//--------------------------------------------------------------------------------------------------------------------------
//
//  将键盘扫描码转成ASCII码
//
//--------------------------------------------------------------------------------------------------------------------------
unsigned char ToASCII(unsigned char keycode)
{
    switch(keycode)
    {
     case 0x1e :
      case 0x9e :  return 'a' ;
      case 0x30 :
      case 0xb0 :  return 'b' ;
      case 0x2e :
      case 0xae :  return 'c' ;
      case 0x20 :
      case 0xa0 :  return 'd' ;
      case 0x12 :
      case 0x92 :  return 'e' ;
      case 0x21 :
      case 0xa1 :  return 'f' ;
      case 0x22 :
      case 0xa2 :  return 'g' ;
      case 0x23 :
      case 0xa3 :  return 'h' ;
      case 0x17 :
      case 0x97 :  return 'i' ;
      case 0x24 :
      case 0xa4 :  return 'j' ;
      case 0x25 :
      case 0xa5 :  return 'k' ;
      case 0x26 :
      case 0xa6 :  return 'l' ;
      case 0x32 :
      case 0xb2 :  return 'm' ;
      case 0x31 :
      case 0xb1 :  return 'n' ;
      case 0x18 :
      case 0x98 :  return 'o' ;
      case 0x19 :
      case 0x99 :  return 'p' ;
      case 0x10 :
      case 0x90 :  return 'q' ;
      case 0x13 :
      case 0x93 :  return 'r' ;
      case 0x1f :
      case 0x9f :  return 's' ;
      case 0x14 :
      case 0x94 :  return 't' ;
      case 0x16 :
      case 0x96 :  return 'u' ;
      case 0x2f :
      case 0xaf :  return 'v' ;
      case 0x11 :
      case 0x91 :  return 'w' ;
      case 0x2d :
      case 0xad :  return 'x' ;
      case 0x15 :
      case 0x95 :  return 'y' ;
      case 0x2c :
      case 0xac :  return 'z' ;
      case 0x0b :
      case 0x8b :  return '0' ;
      case 0x1c :
      case 0x9c :  return '\n' ; 
      case 0x0e :
      case 0x8e :  return '\b' ; // 退格键
     }

		return 1;

}



//--------------------------------------------------------------------------------------------------------------------------
//
//  键盘中断处理
//
//--------------------------------------------------------------------------------------------------------------------------
void KeyboardHandle(void)
{
      KnlMes     mes;
      unsigned char ch=inb(0x60);
      if(ch==0x0) return;
      if(ch>0x80)
      mes.MessageType = MES_KEYUP;
      else
      mes.MessageType = MES_KEYDOWN;  

      mes.Param1= ch;                                                    //参数1为扫描码
      mes.Param2= ToASCII(ch);                                           //参数2为ASCII码
			KnlPutMessage(KnlMesQue,mes);
			
}


//--------------------------------------------------------------------------------------------------------------------------
//
//  键盘驱动初始化
//
//--------------------------------------------------------------------------------------------------------------------------
void KeyboardInit(void)
{
	    ConsolePrintf("Set the keyboard driver");
      SetIntrGate(0x21,(void*)KeyboardInterrupt);
      outb_p(inb_p(0x21)&0xfd,0x21);     
			ConsolePrintOK();
			TickDelay(1);
}

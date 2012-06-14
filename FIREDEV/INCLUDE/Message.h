#ifndef MESSAGE_H
#define MESSAGE_H

#define MesMaxLength 256

#define MES_KEYUP        1
#define MES_KEYDOWN      2
#define MES_TIMER        5
#define MES_MOUSE_MOVE   6
#define MES_MOUSE_LDOWN  7
#define MES_MOUSE_LUP    8
#define MES_MOUSE_RDOWN  9
#define MES_MOUSE_RUP    10
#define MES_MOUSE_MDOWN  11
#define MES_MOUSE_MUP    12
#define MES_ERASE        50
#define MES_WND_MOVE     90
#define MES_WND_ACTIVE   95
#define MES_WND_KILL_ACTIVE  100
#define MES_WND_CREATE  115



typedef struct{
  unsigned char MessageType ;   // 消息类型0~255
	unsigned char hInstance;      //所属进程号,即发送该消息的进程号
	unsigned long hWnd;
  long Param1;         //消息参数1
  long Param2;         //消息参数2
} KnlMes;

extern KnlMes KernelMessage[MesMaxLength];


extern  inline unsigned long SETLONGWORD(short lParam,short hParam)
{
	unsigned long    LWord;
  unsigned long    HWord;
  unsigned long    LONGWord;

  HWord = 0x0000ffff & hParam;
  LWord = 0x0000ffff & lParam;
  HWord = HWord<<16;
  LONGWord= HWord | LWord;

  return  LONGWord;
}

extern inline short LWORD(unsigned long LONGWord)
{
	return (short)(LONGWord);
}

extern inline short HWORD(unsigned long LONGWord)
{
	return (short)(LONGWord>>16);
}


#endif


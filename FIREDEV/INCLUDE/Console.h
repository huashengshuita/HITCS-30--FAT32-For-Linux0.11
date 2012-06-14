#ifndef CONSOLE_H
#define CONSOLE_H

void ConsoleSetTextColor(unsigned long Color);
void ConsoleSetBkColor(unsigned long Color);
void ConsoleScrollDown(int nRow);
void ConsoleScrollUp(int nRow);
void ConsoleInit(int BkColor,int TextColor);
void ConsoleOutAsc(unsigned char ch);
void ConsolePrintOK(void);
void ConsolePrintFault(void);
int  ConsolePrintf(const char * fmt, ...);
int  ConsoleWarning(const char * fmt, ...);
int  ConsoleMsg(const char * fmt, ...);

#endif

/* end of Console.h */

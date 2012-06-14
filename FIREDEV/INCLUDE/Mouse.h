#ifndef MOUSE_H
#define MOUSE_H

#include "GUI.h"

void MouseHandle(void);
void MouseInit(void);
void SetMousePos(int *xChange,int *yChange);
void DrawMouseArrow(int x,int y);
void InitMouseBuf(void);
int  GetMousePosX(void);
int  GetMousePosY(void);
void RefreshMouseBuf(RECT aInvRect);

#endif

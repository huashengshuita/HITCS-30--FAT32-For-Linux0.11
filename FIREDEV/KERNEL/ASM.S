[BITS 32]
[extern _DoDivideError]
[extern _DoDebug]
[extern _DoNmi]
[extern _DoInt3]
[extern _DoOverFlow]
[extern _DoBounds]
[extern _DoInvalidOp]
[extern _DoCoprocessorSegmentOverrun]
[extern _DoReserved]
[extern _DoDoubleFault]
[extern _DoInvalidTSS]
[extern _DoGeneralProtection]
[extern _DoPageFault]
[extern _DoSegmentNotPresent]
[extern _DoStackSegment]
[extern _DoDeviceNotAvailable]
[extern _DoCoprocessorError]
[extern _DoNoPage]
[extern _DoWpPage]

[global _DivideError]
[global _Debug]
[global _Nmi]
[global _Int3]
[global _OverFlow]
[global _Bounds]
[global _InvalidOp]
[global _CoprocessorSegmentOverrun]
[global _Reserved]
[global _DoubleFault]
[global _InvalidTSS]
[global _GeneralProtection]
[global _PageFault]
[global _SegmentNotPresent]
[global _StackSegment]
[global _DeviceNotAvailable]
[global _CoprocessorError]

[SECTION .text]
;int 0 (被零除出错处理)
_DivideError:  
      push _DoDivideError               ;将出被零除出错处理函数地址入栈
no_error_code:
      xchg [esp] , eax                  ;将eax 和 当前esp所指内容交换,也即把 eax 入栈,将 _do_divide_error --> eax
      push ebx
      push ecx
      push edx
      push edi
      push esi
      push ebp
      push ds
      push es
      push fs
      push 0                            ;出错号为 0 : 无错码
      lea  edx , [esp+44]               ;使指向中断返回地址入栈位置处的esp0入栈
      push edx 
      mov  edx , 0x18
      mov  ds  , edx
      mov  es  , edx
      mov  fs  , edx
      call eax                               ;调用do_divide_error函数 
      add  esp , 8                         ;使esp指向fs入栈位置处
      pop  fs
      pop  es
      pop  ds
      pop  ebp
      pop  esi
      pop  edi
      pop  edx
      pop  ecx
      pop  ebx
      pop  eax  
      iret

;int 1 -- debug 调试入点 属于无出错码中断
_Debug:
      push _DoInt3                    ;将出调试入点函数地址入栈
      jmp  no_error_code

;int 2 -- 非屏蔽中断出错  属于无出错码中断
_Nmi:
      push _DoNmi
      jmp no_error_code

;int 3 -- 同debug        属于无出错码中断
_Int3:
      push _DoInt3                        ;将出调试入点函数地址入栈
      jmp  no_error_code

;int 4 -- 溢出出错         属于无出错码中断
_OverFlow:
      push _DoOverFlow                    ;将出溢出出错函数地址入栈
      jmp  no_error_code


;int 5 -- 边界检查出错        属于无出错码中断
_Bounds:
      push _DoBounds                                   ;将边界检查出错函数地址入栈
      jmp  no_error_code

;int 6 -- 无效指令出错        属于无出错码中断
_InvalidOp:
      push _DoInvalidOp                              ;将无效指令出错函数地址入栈
      jmp  no_error_code

;int 9 -- 协处理器段超出出错        属于无出错码中断
_CoprocessorSegmentOverrun:
      push _DoCoprocessorSegmentOverrun                ;将边界检查出错函数地址入栈
      jmp  no_error_code

;int 15 -- 保留出错              属于无出错码中断
_Reserved:
      push _DoReserved                                                ;将保留出错函数地址入栈
      jmp  no_error_code

;int45(0x20+13) 协处理器发出的中断
;_irq13:
    
;       push eax
;       xor   al   , al
;       out   0xF0 , al
;       mov   al   , 0x20
;       out   0x20 , al
;       jmp   _delay_1
;_delay_1:
;       jmp   _delay_2
;_delay_2:
;       out   0xA0 , al
;       pop   eax
;       jmp   _coprocesser_error

;int8 -- 双出错故障
;属于有出错号中断
;有出错号中断时,系统会把出错号入栈
_DoubleFault:
        push _DoDoubleFault
error_code:
        xchg eax , [esp+4]               ;将出错号入 --> eax
        xchg ebx , [esp]                 ;中断地址 -->   ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp
        push ds
        push es
        push fs
        push eax                             ;出错号入栈
        lea  eax , [esp+44]              ;使指向中断返回地址入栈位置处的esp0入栈
        push eax
        mov  eax , 0x18
        mov  ds  , eax
        mov  es  , eax
        mov  fs  , eax
        call ebx
        add  esp , 8
        pop  fs
        pop  es
        pop  ds
        pop  ebp
        pop  esi
        pop  edi
        pop  edx
        pop  ecx
        pop  ebx
        pop  eax  
        iret

;int 10 -- 无效的任务状态段 (TSS)
_InvalidTSS:
        push _DoInvalidTSS
        jmp  error_code

;int 13 -- 一般性保护出错
_GeneralProtection:
        push _DoGeneralProtection
        jmp  error_code
        
;_PageFault:
;        push _DoPageFault
;        jmp  error_code
        
_SegmentNotPresent:
        push _DoSegmentNotPresent
        jmp  error_code
        
_StackSegment:
        push _DoStackSegment
        jmp  error_code
        
_DeviceNotAvailable:
        push _DoDeviceNotAvailable
        jmp  error_code
        
_CoprocessorError:
        push _DoCoprocessorError
        jmp  error_code
        
;该文件包括页异常中断处理程序（中断14），主要分两种情况处理。一是由于缺页引起的页异常中断，
 ; 通过调用do_no_page(error_code, address)来处理；二是其它页保护引起的页异常，此时调用页写
 ; 保护处理函数do_wp_page(error_code, address)进行处理。其中的出错码(error_code)是由CPU 自
 ; 动产生并压入堆栈的，出现异常时访问的线性地址是从控制寄存器CR2 中取得的。

_PageFault:
 xchg eax , [esp]               ;将出错号入 --> eax
 push ecx
 push edx
 push ds
 push es
 push fs
 mov  edx , 0x18              ;置内核数据段选择符。
 mov  ds  , edx
 mov  es  , edx
 mov  fs  , edx
 mov  edx , cr2               ;取引起页面异常的线性地址
 push edx                     ;将该线性地址和出错码压入堆栈，作为调用函数的参数。
 push eax
 test eax , 1                 ;测试标志P，如果不是缺页引起的异常则跳转。
 jne  PageProtect
 call _DoNoPage               ;调用缺页处理函数（mm/memory.c,365 行）。
 jmp  PageLost
 
PageProtect:
 call _DoWpPage               ;调用写保护处理函数（mm/memory.c,247 行）。
  
PageLost:  
  add  esp,8                  ;丢弃压入栈的两个参数。
  pop  fs
  pop  es
  pop  ds
  pop  edx
  pop  ecx
  pop  eax
  iret
        
        
 
        


        
        

        


       
      






     

     
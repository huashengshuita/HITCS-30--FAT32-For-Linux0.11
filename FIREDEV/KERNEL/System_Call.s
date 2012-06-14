[BITS 32]
[extern _TimerHandle]
[global _TimerInterrupt]

[extern _KeyboardHandle]
[global _KeyboardInterrupt]

[extern _MouseHandle]
[global _MouseInterrupt]

[extern _FloppyHandle]
[global _FloppyInterrupt]

[extern _HardDiskHandle]
[global _HardDiskInterrupt]

[extern _syscall_table]
[global _sys_call]



[global _ret_from_sys_call]

; Stack layout in 'ret_from_system_call':
;
;	 0(%esp) - %eax
;	 4(%esp) - %ebx
;	 8(%esp) - %ecx
;	 C(%esp) - %edx
;	10(%esp) - %fs
;	14(%esp) - %es
;	18(%esp) - %ds
;	1C(%esp) - %eip
;	20(%esp) - %cs
;	24(%esp) - %eflags
;	28(%esp) - %oldesp                      当特权级有变化时,系统会把原来用户空间或系统空间的堆栈段和堆栈指针入栈
;	2C(%esp) - %oldss
;


_ret_from_sys_call:
         
      ;mov  eax , _current 
      ;cmp  eax , _task
      ;je 3
      ;jmp   3f                                ;此处我们还没有引入进程信号量,先跳过信号量的处理
      ;cmp  [esp+0x20] , 0x0f                  ;是否为普通用户代码段?
      ;jne   3f
       
       ;.............
       popa			; save "general" registers

       pop	gs		; save gs
       pop	fs		; save fs
       pop	es		; save es
       pop	ds		; save ds

       add esp, 8
       sti

       iret

_TimerInterrupt:      
       cli
       push     0
       push     eax

       push	ds		; save ds
       push	es		; save es
       push	fs		; save fs
       push	gs		; save gs
       pusha		; save "general" registers	

       mov   eax  ,  0x18           ;使ds,es指向内核数据段
       mov   ds   ,  eax
       mov   es   ,  eax
       mov   al   ,  0x20
       out   0x20 ,  al            ;送EOI到 0x20端口
       mov   eax  ,  [esp+0x20]    ;取出CS选则段
       and   eax  ,  3             ;是否为超级用户(是则 eax =0 否则为 3)
       push  eax
       call  _TimerHandle
       add   esp  , 4
       jmp   _ret_from_sys_call
       
_KeyboardInterrupt:
       cli
       push     0
       push     eax

       push	ds		; save ds
       push	es		; save es
       push	fs		; save fs
       push	gs		; save gs
       pusha		; save "general" registers	

       mov   eax , 0x18           ;使ds,es指向内核数据段
       mov   ds  , eax
       mov   es  , eax
       ;mov   eax , 0x17
       ;mov   fs  , eax              ;使fs指向出错程序数据段
       mov   al   , 0x20
       out   0x20 , al               ;送EOI到 0x20端口
       call  _KeyboardHandle
       jmp   _ret_from_sys_call
       
       
_MouseInterrupt:
       cli
       push     0
       push     eax

       push	ds		; save ds
       push	es		; save es
       push	fs		; save fs
       push	gs		; save gs
       pusha		; save "general" registers	

       mov   eax , 0x18           ;使ds,es指向内核数据段
       mov   ds  , eax
       mov   es  , eax
       mov   al   , 0x20
       out   0x20 , al               ;送EOI到 0x20端口
       out   0xa0 , al
       call  _MouseHandle
       jmp   _ret_from_sys_call
       
_FloppyInterrupt:
       cli

       push     0
       push     eax

       push  ds
       push  es
       push  fs
       push  gs
       pusha			; save "general" registers	

       mov   eax , 0x18           ;使ds,es指向内核数据段
       mov   ds  , eax
       mov   es  , eax
       mov   al  , 0x20
       out   0x20 , al             ;送EOI到 0x20端口
       call  _FloppyHandle
       jmp   _ret_from_sys_call
       
_HardDiskInterrupt:
       cli

       push     0
       push     eax

       push  ds
       push  es
       push  fs
       push  gs
       pusha			; save "general" registers	

       mov   eax , 0x18           ;使ds,es指向内核数据段
       mov   ds  , eax
       mov   es  , eax
       mov   al  , 0x20
       out   0x20 , al             ;送EOI到 0x20端口
       out   0xa0 , al
       call  _HardDiskHandle
       jmp   _ret_from_sys_call
       
_sys_call:
       cli
       push     2
       push     eax

       push	ds		; save ds
       push	es		; save es
       push	fs		; save fs
       push	gs		; save gs
       pusha		; save "general" registers	
	
       push     edx
       mov      edx , 0x18           ;使ds,es指向内核数据段
       mov      ds  , edx
       mov      es  , edx
       mov      edx , 0x2B
       mov      gs  , edx
       pop      edx

       call [_syscall_table + 4*eax]
       jmp   _ret_from_sys_call




       
      
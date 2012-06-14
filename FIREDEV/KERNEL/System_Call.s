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
;	28(%esp) - %oldesp                      ����Ȩ���б仯ʱ,ϵͳ���ԭ���û��ռ��ϵͳ�ռ�Ķ�ջ�κͶ�ջָ����ջ
;	2C(%esp) - %oldss
;


_ret_from_sys_call:
         
      ;mov  eax , _current 
      ;cmp  eax , _task
      ;je 3
      ;jmp   3f                                ;�˴����ǻ�û����������ź���,�������ź����Ĵ���
      ;cmp  [esp+0x20] , 0x0f                  ;�Ƿ�Ϊ��ͨ�û������?
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

       mov   eax  ,  0x18           ;ʹds,esָ���ں����ݶ�
       mov   ds   ,  eax
       mov   es   ,  eax
       mov   al   ,  0x20
       out   0x20 ,  al            ;��EOI�� 0x20�˿�
       mov   eax  ,  [esp+0x20]    ;ȡ��CSѡ���
       and   eax  ,  3             ;�Ƿ�Ϊ�����û�(���� eax =0 ����Ϊ 3)
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

       mov   eax , 0x18           ;ʹds,esָ���ں����ݶ�
       mov   ds  , eax
       mov   es  , eax
       ;mov   eax , 0x17
       ;mov   fs  , eax              ;ʹfsָ�����������ݶ�
       mov   al   , 0x20
       out   0x20 , al               ;��EOI�� 0x20�˿�
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

       mov   eax , 0x18           ;ʹds,esָ���ں����ݶ�
       mov   ds  , eax
       mov   es  , eax
       mov   al   , 0x20
       out   0x20 , al               ;��EOI�� 0x20�˿�
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

       mov   eax , 0x18           ;ʹds,esָ���ں����ݶ�
       mov   ds  , eax
       mov   es  , eax
       mov   al  , 0x20
       out   0x20 , al             ;��EOI�� 0x20�˿�
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

       mov   eax , 0x18           ;ʹds,esָ���ں����ݶ�
       mov   ds  , eax
       mov   es  , eax
       mov   al  , 0x20
       out   0x20 , al             ;��EOI�� 0x20�˿�
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
       mov      edx , 0x18           ;ʹds,esָ���ں����ݶ�
       mov      ds  , edx
       mov      es  , edx
       mov      edx , 0x2B
       mov      gs  , edx
       pop      edx

       call [_syscall_table + 4*eax]
       jmp   _ret_from_sys_call




       
      
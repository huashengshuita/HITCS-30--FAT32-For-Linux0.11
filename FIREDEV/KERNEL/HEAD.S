[BITS 32]
[extern _k_main]
[global _gdt] 
[global _idt]
[global start]

;;
;主入口点
start:
    cli
       
    ;; 重新设置所有的东西
	mov   eax, 0x18
	mov   ds,  eax
	mov   es,  eax
	;mov   ss , eax
	mov   fs,  eax
	mov   gs,  eax
	xor   eax, eax
	;; 设置内核堆栈，从0x80000处自上往下生长
	mov   esp, 0x80000
	
	lgdt  [gdt_pdesc-0xC0000000]
	;lgdt  [gdt_pdesc]
	
	call  Init8259A
	call  Load_IDT
	
    call  _k_main                   ;去我们的C语言入口点了!!    


[global Init8259A]
Init8259A:
       ;; 重新初始化8259中断控制器
	;; 初始化顺序
	mov   al, 0x11	
	out   0x20, al
	call  _delay
	out   0xA0, al
	call  _delay
	;; 设置硬件中断开始处 (0x20).
	mov   al, 0x20
	out   0x21, al
	call  _delay
	;; 设置硬件中断开始处 (0x28).
	mov   al, 0x28
	out   0xA1, al
	call  _delay
	;; 设置主片
	mov   al, 0x04
	out   0x21, al
	call  _delay
	;; 设置从片
	mov   al, 0x02
	out   0xA1, al
	call  _delay
	;; 8086 mode.
	mov   al, 0x01
	out   0x21, al
	call  _delay
	out   0xA1, al
	call  _delay
	;; 屏蔽所有
	mov   al, 0xFB
	out   0x21, al
	call  _delay
    mov   al,0xFF
	out   0xA1, al
    ret

[global Load_IDT]
Load_IDT:
	;; 设置idt,并将ignore_int设成所有的中断处理函数
	;mov   edx, ignore_int-0xC0000000
	mov   edx, ignore_int
	mov   eax, 0x00100000
	mov   ax,  dx
	mov   dx, 0x8E00
	mov   edi, _idt
	mov   ecx, 256
rp_sidt:
	mov   [edi],   eax
	mov   [4+edi], edx
	add   edi, 8
	dec   ecx
	jne   rp_sidt
	lidt  [idt_pdesc-0xC0000000]
	;lidt  [idt_pdesc]
    ret

;仅仅用来产生一些延迟
[global _delay]
_delay:
       ret

[global ignore_int]
ignore_int:
	;; 默认中断处理程序，什么也不做
	iret

align 2
idt_pdesc:			            ; idt伪描述符
	dw 256 * 8 - 1	            ; idt contains 256 entries
	dd _idt

	align 2
gdt_pdesc:			            ; gdt伪描述符
	dw 256 * 8 - 1	            ; But I think we must large it.
	dd _gdt		                ; Or only use one ldt and tss 
				                ; in our system. 
	align 8

_idt:
	times 	256 * 8 db 0        ;中断门暂时为空

	align 3                     ;四个段 一个空段 一个核心代码段 一个核心数据段 一个显存段 一个页表目录段
_gdt:  
  dw 0, 0, 0, 0                 ;null (0)
  
  dw 0, 0, 0, 0                 ;null (0)

                                ;kernel cs 0x10 (1)
  dw 0xFFFF	                    ;base: 0, limit: 4G
  dw 0x0000
  dw 0x9A00                     ;Type: present,ring0,data/stack,read/write (10010010)
  dw 0x00CF                     ;Limit(1):0xF | Flags:4Kb inc,32bit (11001111)    F --->段限高4位

	                            ;kernel ds 0x18 (2)
  dw 0xFFFF	                    ;base: 0, limit: 4G
  dw 0x0000
  dw 0x9200
  dw 0x00CF
                                ;user  cs 0x23  (3)   11    0-在GDT中 11 特权级为 3
  dw 0xFFFF	                    ;base: 0, limit: 4G
  dw 0x0000
  dw 0xFA00	 
  dw 0x00CF

	                            ;user ds 0x2B   (4)    100  0-在GDT中 11 特权级为 3
  dw 0xFFFF	                    ;base: 0, limit: 4G
  dw 0x0000
  dw 0xF200
  dw 0x00CF

;tss descriptor
;dw 0, 0, 0, 0

  times 1024              db 0





   




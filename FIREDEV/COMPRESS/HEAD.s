;;
;;
;;  FireX OS  0.01
;;  Copyright (C) 2003  XiaoFan     All rights reserved.     
;;
    SECTION .text
    BITS 32

    GLOBAL  _start                                ;4 init shell
    EXTERN  _decompress_kernel, _error_boot       ;jump to init code
    
;================================================================
    align 4
    EXTERN code, bss, end
    
	; these are in the linker script
 
 _start:
    cmp eax, 0x2BADB002           ;检查是否是从GRUB引导的
	je __boot_from_grub

	push edx
	push ecx
	push ebx
	push eax
	call _error_boot
	mov word [0B8000h], 144ah
	jmp short $

;================================================================
__boot_from_grub:

    lgdt  [gdt_pdesc]               ;重载GDT表
    
    jmp dword 0x10:LOAD_GDT      
    
LOAD_GDT:
    
    mov   eax, 0x18
    mov   ds , eax
	mov   es , eax
	mov   ss , eax
	mov   fs , eax
	mov   gs , eax
	xor   eax, eax
	mov	  esp, boot_stack
	
	push	dword 0
	popf                              ;Make sure the eflag register be 0

	; ebx holds a pointer to
	; the multiboot information block	
	push ebx
	push eax
	mov eax, 0
	cpuid
	push edx
	
    call _decompress_kernel		       ; 内核自解压
    
    
    mov   eax, 0x18
    mov   ds , eax
	mov   es , eax
	mov   ss , eax
	mov   fs , eax
	mov   gs , eax
	xor   eax, eax
	
    jmp dword 0x10:0x0                 ;跳到解压后的内核去运行
    
    jmp short $
    
	
;=================================================================
;COFF文件重定位

	ALIGN  4
	grub_jicama:
	dd 0x1BADB002
	dd 0x00010000
	dd 0-0x1BADB002-0x00010000
    ; aout kludge. These must be PHYSICAL addresses
	dd grub_jicama
	dd code
	dd bss
	dd end
	dd _start

	dd 0
	dd 800
	dd 600
	dd 32
	
;=================================================================
;GDT TABLE

align 2

gdt_pdesc:			       
	dw 4 * 8 - 1		
	dd _gdt			

align 3                   

_gdt:  
	dw 0, 0, 0, 0		        ;    (0)
	dw 0, 0, 0, 0		        ;    (0)
	
	                            ;kernel cs 0x08 (1)
	DW     0xFFFF
    DW     0x0
    DB     0x0
    DB     0x9A                 ; Type: present,ring0,code,exec/read/accessed (10011000)
    DB     0xCF                 ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
    DB     0x0

	                            ; kernel ds 0x10 (2)
	DW     0xFFFF
    DW     0x0
    DB     0x0
    DB     0x92                 ; Type: present,ring0,data/stack,read/write (10010010)
    DB     0xCF                 ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
                    
;==================================================================
;数据段

SECTION .data

	global	_ASC, _GB2312,_VmKnl,gdt
	bits 32

	_ASC:
	incbin './ascii.fon'             ;英文字库
	
	_VmKnl:
	incbin './vmKnl.bin'
	
	

;==================================================================
;BSS段

;SECTION .bss
;	align	16
;_ktss
;	resb 	128

;	ALIGNB  4
;%IFDEF DEBUG
;	RESB	4096
;%ELSE
;	RESB	1024
;%ENDIF
boot_stack:


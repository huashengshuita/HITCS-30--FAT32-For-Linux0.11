;; 文件名：  font.asm
;; 描述：    此文件包含中英文字库文件
	SECTION .data
	global	_ASC
	bits32
_ASC:
	incbin 'driver/asc16'

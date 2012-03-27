; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               start.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2008
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

; 导入函数
extern	main
extern	exit

bits 32

[section .text]	; 代码在此

global _start	; 导出 _start

_start:				; default entrance
	push	eax
	push	ecx
	call	main
	;; need not clean up the stack here

	push	eax
	call	exit
	hlt			; should never arrive here


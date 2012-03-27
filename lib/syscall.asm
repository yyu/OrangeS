
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;; 增加一个系统调用的 TODO-list
;; 				TODO					file
;; 1. NR_SYS_CALL++							const.h
;; 2. 为 sys_call_table[] 增加一个新成员，假设为 sys_foo		global.c
;; 3. sys_foo 的函数体							<it depends>
;;        sys_printx:  tty.c
;;        sys_sendrec: proc.c
;; 4. sys_foo 的函数声明						proto.h
;; 5. foo 的函数声明							proto.h
;; 6. _NR_foo 的定义							syscall.asm
;; 7. foo 的函数体							syscall.asm
;; 8. 添加 global foo							syscall.asm
;; 9. 如果参数个数与以前的系统调用比有所增加，则需要修改 sys_call	kernel.asm


INT_VECTOR_SYS_CALL	equ	0x90
_NR_printx		equ	0
_NR_sendrec		equ	1

; 导出符号
global	sendrec
global	printx

bits 32
[section .text]

; ====================================================================================
;                  sendrec(int function, int src_dest, MESSAGE* p_msg);
; ====================================================================================
; Never call sendrec() directly, call send_recv() instead.
sendrec:
	push	ebx		; .
	push	ecx		;  > 12 bytes
	push	edx		; /

	mov	eax, _NR_sendrec
	mov	ebx, [esp + 12 +  4]	; function
	mov	ecx, [esp + 12 +  8]	; src_dest
	mov	edx, [esp + 12 + 12]	; msg
	int	INT_VECTOR_SYS_CALL

	pop	edx
	pop	ecx
	pop	ebx

	ret

; ====================================================================================
;                          void printx(char* s);
; ====================================================================================
printx:
	push	edx		; 4 bytes

	mov	eax, _NR_printx
	mov	edx, [esp + 4 + 4]	; s
	int	INT_VECTOR_SYS_CALL

	pop	edx

	ret

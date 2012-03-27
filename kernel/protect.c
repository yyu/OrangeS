/*************************************************************************//**
 *****************************************************************************
 * @file   protect.c
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "string.h"

PRIVATE void init_idt_desc(u8 vec, u8 type, int_handler handler, u8 priv);


/* interrupt handlers defined in kernel.asm */
void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();


/*****************************************************************************
 *                                init_prot
 *****************************************************************************/
/**
 * This routine:
 *  -# Initializes IDT (Interrupt Descriptor Table).
 *  -# Fills the TSS descriptor in GDT.
 *  -# Fills the LDT descriptors of each proc in GDT.
 *****************************************************************************/
PUBLIC void init_prot()
{
	init_8259A();

	/* Every entry in the IDT is Interrupt Gate (No Trap Gate) */
	init_idt_desc(INT_VECTOR_DIVIDE,	DA_386IGate,
		      divide_error,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_DEBUG,		DA_386IGate,
		      single_step_exception,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_NMI,		DA_386IGate,
		      nmi,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_BREAKPOINT,	DA_386IGate,
		      breakpoint_exception,	PRIVILEGE_USER);

	init_idt_desc(INT_VECTOR_OVERFLOW,	DA_386IGate,
		      overflow,			PRIVILEGE_USER);

	init_idt_desc(INT_VECTOR_BOUNDS,	DA_386IGate,
		      bounds_check,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_INVAL_OP,	DA_386IGate,
		      inval_opcode,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_COPROC_NOT,	DA_386IGate,
		      copr_not_available,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate,
		      double_fault,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_COPROC_SEG,	DA_386IGate,
		      copr_seg_overrun,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_INVAL_TSS,	DA_386IGate,
		      inval_tss,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_SEG_NOT,	DA_386IGate,
		      segment_not_present,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_STACK_FAULT,	DA_386IGate,
		      stack_exception,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_PROTECTION,	DA_386IGate,
		      general_protection,	PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_PAGE_FAULT,	DA_386IGate,
		      page_fault,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_COPROC_ERR,	DA_386IGate,
		      copr_error,		PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 0,	DA_386IGate,
		      hwint00,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 1,	DA_386IGate,
		      hwint01,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 2,	DA_386IGate,
		      hwint02,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 3,	DA_386IGate,
		      hwint03,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 4,	DA_386IGate,
		      hwint04,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 5,	DA_386IGate,
		      hwint05,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 6,	DA_386IGate,
		      hwint06,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ0 + 7,	DA_386IGate,
		      hwint07,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 0,	DA_386IGate,
		      hwint08,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 1,	DA_386IGate,
		      hwint09,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 2,	DA_386IGate,
		      hwint10,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 3,	DA_386IGate,
		      hwint11,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 4,	DA_386IGate,
		      hwint12,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 5,	DA_386IGate,
		      hwint13,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 6,	DA_386IGate,
		      hwint14,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_IRQ8 + 7,	DA_386IGate,
		      hwint15,			PRIVILEGE_KRNL);

	init_idt_desc(INT_VECTOR_SYS_CALL,	DA_386IGate,
		      sys_call,			PRIVILEGE_USER);

	/* Fill the TSS descriptor in GDT */
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = SELECTOR_KERNEL_DS;
	init_desc(&gdt[INDEX_TSS],
		  makelinear(SELECTOR_KERNEL_DS, &tss),
		  sizeof(tss) - 1,
		  DA_386TSS);
	tss.iobase = sizeof(tss); /* No IO permission bitmap */

	/* Fill the LDT descriptors of each proc in GDT  */
	int i;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++) {
		memset(&proc_table[i], 0, sizeof(struct proc));

		proc_table[i].ldt_sel = SELECTOR_LDT_FIRST + (i << 3);
		assert(INDEX_LDT_FIRST + i < GDT_SIZE);
		init_desc(&gdt[INDEX_LDT_FIRST + i],
			  makelinear(SELECTOR_KERNEL_DS, proc_table[i].ldts),
			  LDT_SIZE * sizeof(struct descriptor) - 1,
			  DA_LDT);
	}
}


/*****************************************************************************
 *                                init_idt_desc
 *****************************************************************************/
/**
 * Initialize an IDT descriptor.
 * 
 * @param vec      Interrupt vector nr.
 * @param type     Task-gate, Interrupt-gate or Trap-gate.
 * @param handler  The interrupt handler.
 * @param priv     Priviledge.
 *****************************************************************************/
PRIVATE void init_idt_desc(u8 vec, u8 type, int_handler handler, u8 priv)
{
	struct gate * p_gate = &idt[vec];
	u32 base = (u32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= SELECTOR_KERNEL_CS;
	p_gate->dcount		= 0;
	p_gate->attr		= type | (priv << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}


/*****************************************************************************
 *                                seg2linear
 *****************************************************************************/
/**
 * Calculate the linear address of a given segment.
 * 
 * @param seg  Selector in GDT.
 * 
 * @return  The linear address of the segment base.
 *****************************************************************************/
PUBLIC u32 seg2linear(u16 seg)
{
	struct descriptor * d = &gdt[seg >> 3];

	return (d->base_high << 24) | (d->base_mid << 16) | (d->base_low);
}


/*****************************************************************************
 *                                init_desc
 *****************************************************************************/
/**
 * Initialize a segment descriptor.
 * 
 * @param d     The descriptor ptr.
 * @param base  Base of the segment.
 * @param lim   Limit of the segment. (base + limit -> last valid byte)
 * @param attr  Attribution.
 *****************************************************************************/
PUBLIC void init_desc(struct descriptor * d, u32 base, u32 lim, u16 attr)
{
	d->limit_low		= lim & 0x0FFFF;
	d->base_low		= base & 0x0FFFF;
	d->base_mid		= (base >> 16) & 0x0FF;
	d->attr1		= attr & 0xFF;
	d->limit_high_attr2	= ((lim >> 16) & 0x0F) | ((attr >> 8) & 0xF0);
	d->base_high		= (base >> 24) & 0x0FF;
}

/*======================================================================*
                            exception_handler
 *----------------------------------------------------------------------*
 异常处理
 *======================================================================*/
/*****************************************************************************
 *                                exception_handler
 *****************************************************************************/
/**
 * This routine will be invoked when any exception occurs.
 * @see kernel.asm
 * 
 * @param vec_no    Interrupt vector nr.
 * @param err_code  Error code. If an exception has no error code, it'll be
 *                  some value defined in kernel.asm.
 * @param eip       Pushed by cpu, the eip when the exception occurs.
 * @param cs        Pushed by cpu, the cs when the exception occurs.
 * @param eflags    Pushed by cpu, the eflags when the exception occurs.
 *****************************************************************************/
PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
	char * err_msg[] = {"#DE Divide Error",
			    "#DB RESERVED",
			    "—  NMI Interrupt",
			    "#BP Breakpoint",
			    "#OF Overflow",
			    "#BR BOUND Range Exceeded",
			    "#UD Invalid Opcode (Undefined Opcode)",
			    "#NM Device Not Available (No Math Coprocessor)",
			    "#DF Double Fault",
			    "    Coprocessor Segment Overrun (reserved)",
			    "#TS Invalid TSS",
			    "#NP Segment Not Present",
			    "#SS Stack-Segment Fault",
			    "#GP General Protection",
			    "#PF Page Fault",
			    "—  (Intel reserved. Do not use.)",
			    "#MF x87 FPU Floating-Point Error (Math Fault)",
			    "#AC Alignment Check",
			    "#MC Machine Check",
			    "#XF SIMD Floating-Point Exception"
	};

	char estr[STR_DEFAULT_LEN];
	sprintf(estr, "Exception: %s: eflags: %xh, cs:%xh, eip:%xh",
		err_msg[vec_no], eflags, cs, eip);

	if (err_code != 0x7FFFFFFF) { /**< @see kernel.asm::NO_ERR_CODE */
		char err_code_str[32];
		sprintf(err_code_str, ", err code: %xh", err_code);
		strcat(estr, err_code_str);
	}

	panic(estr);
}


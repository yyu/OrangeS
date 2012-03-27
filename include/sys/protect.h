/*************************************************************************//**
 *****************************************************************************
 * @file   protect.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

#ifndef	_ORANGES_PROTECT_H_
#define	_ORANGES_PROTECT_H_

/**
 * @struct descriptor
 * Segment descriptor.
 */
struct descriptor /* 8 bytes */
{
	u16	limit_low;	  /* Limit */
	u16	base_low;	  /* Base */
	u8	base_mid;	  /* Base */
	u8	attr1;		  /* P(1) DPL(2) DT(1) TYPE(4) */
	u8	limit_high_attr2; /* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	u8	base_high;	  /* Base */
};

#define	reassembly(high, high_shift, mid, mid_shift, low)	\
	(((high) << (high_shift)) +				\
	 ((mid)  << (mid_shift)) +				\
	 (low))

/**
 * @struct gate
 * Gate descriptor.
 */
struct gate
{
	u16 offset_low;	/**< Offset Low */
	u16 selector;	/**< Selector */
	u8  dcount; /**
		     * Only available in Call Gate.
		     * This field indicates the number of parameters
		     * to copy from the calling procedures stack to the
		     * new stack if a stack switch occurs.
		     */
	u8  attr;   /**< P(1) DPL(2) DT(1) TYPE(4) */
	u16 offset_high; /**< Offset High */
};

/**
 * @struct tss
 * Task-State Segment.
 *
 * When the control is transferred from Ring 1,2,3 to Ring 0,
 * esp0 and ss0 will be used to switch the stack.
 *
 * Other fields are not used. 
 */
struct tss {
	u32	backlink;
	u32	esp0;	/* stack pointer used during interrupts */
	u32	ss0;	/*   "   segment  "    "        "       */
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	flags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt;
	u16	trap;
	u16	iobase; /* If the I/O bit map base address is
			 * greater than or equal to the TSS segment
			 * limit, there is no I/O permission map
			 */
	/*u8	iomap[2];*/
};

/* GDT */
/* descriptor indices */
#define	INDEX_DUMMY		0	/* \                      */
#define	INDEX_FLAT_C		1	/* | determined in LOADER */
#define	INDEX_FLAT_RW		2	/* |                      */
#define	INDEX_VIDEO		3	/* /                      */
#define	INDEX_TSS		4
#define	INDEX_LDT_FIRST		5
/* selectors */
#define	SELECTOR_DUMMY		   0		/* \                       */
#define	SELECTOR_FLAT_C		0x08		/* | determined in  LOADER */
#define	SELECTOR_FLAT_RW	0x10		/* |                       */
#define	SELECTOR_VIDEO		(0x18+3)	/* / <-- RPL=3             */
#define	SELECTOR_TSS		0x20		/* TSS */
#define SELECTOR_LDT_FIRST	0x28

#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO

/* every task has it's own LDT, which contains this amount of descriptors: */
#define LDT_SIZE		2
/* descriptor indices in LDT */
#define	INDEX_LDT_C		0
#define	INDEX_LDT_RW		1

/*
 * descriptor types
 */
#define	DA_32		0x4000	/* 32-bit segment */
#define	DA_LIMIT_4K	0x8000	/* When the G flag is set, the processor
				 * scales the value in the limit field by
				 * a factor of 2^12 (4 KBytes).
				 */
#define	LIMIT_4K_SHIFT	  12
#define	DA_DPL0		0x00	/* DPL = 0 */
#define	DA_DPL1		0x20	/* DPL = 1 */
#define	DA_DPL2		0x40	/* DPL = 2 */
#define	DA_DPL3		0x60	/* DPL = 3 */
/*
 * Code- and Data-Segment Descriptor Types
 */
#define	DA_DR		0x90	/* present read-only  data segment */
#define	DA_DRW		0x92	/* present read/write data segment */
#define	DA_DRWA		0x93	/* present accessed read/write data segment */
#define	DA_C		0x98	/* present execute-only code segment */
#define	DA_CR		0x9A	/* present execute-read code segment */
#define	DA_CCO		0x9C	/* present execute-only conforming code segment */
#define	DA_CCOR		0x9E	/* present execute/read-only conforming code segment */
/*
 * System Descriptor Types
 */
#define	DA_LDT		0x82	/* LDT */
#define	DA_TaskGate	0x85	/* Task Gate */
#define	DA_386TSS	0x89	/* 32-bit TSS (Available) */
#define	DA_386CGate	0x8C	/* 32-bit Call Gate */
#define	DA_386IGate	0x8E	/* 32-bit Interrupt Gate */
#define	DA_386TGate	0x8F	/* 32-bit Trap Gate */

/* Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4

/* Interrupt Vectors */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

#define	INT_VECTOR_IRQ0			0x20
#define	INT_VECTOR_IRQ8			0x28

/* System call Interrupt Vector */
#define	INT_VECTOR_SYS_CALL		0x90


/* seg:off -> linear addr */
#define makelinear(seg,off) (u32)(((u32)(seg2linear(seg))) + (u32)(off))


#endif /* _ORANGES_PROTECT_H_ */

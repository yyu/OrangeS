/*************************************************************************//**
 *****************************************************************************
 * @file   i8259.c
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "fs.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "string.h"


/*****************************************************************************
 *                                init_8259A
 *****************************************************************************/
/**
 * Initialize 8259A interrupt controller.
 * 
 *****************************************************************************/
PUBLIC void init_8259A()
{
	/* Master 8259, ICW1. */
	out_byte(INT_M_CTL,	0x11);

	/* Slave  8259, ICW1. */
	out_byte(INT_S_CTL,	0x11);

	/* Master 8259, ICW2. (set the INT vector nr of Master 8259) */
	out_byte(INT_M_CTLMASK,	INT_VECTOR_IRQ0);

	/* Slave  8259, ICW2. (set the INT vector nr of Slave  8259) */
	out_byte(INT_S_CTLMASK,	INT_VECTOR_IRQ8);

	/**
	 * cascade the slave 8259 by connecting the INT line of
	 * the slave to the IRQ line 2 of the master
	 */
	/* Master 8259, ICW3. (bit1 is 1: slave is cascaded to IR2) */
	out_byte(INT_M_CTLMASK,	0x4);
	/* Slave  8259, ICW3. (set to 2: cascaded to IR2 of master) */
	out_byte(INT_S_CTLMASK,	0x2);

	/* Master 8259, ICW4. (80386 mode) */
	out_byte(INT_M_CTLMASK,	0x1);

	/* Slave  8259, ICW4. (80386 mode) */
	out_byte(INT_S_CTLMASK,	0x1);

	/* Master 8259, OCW1. */
	/* cascade IRQ is enabled so that AT winchester can accept interrupt */
	out_byte(INT_M_CTLMASK,	0xFB);

	/* Slave  8259, OCW1. */
	out_byte(INT_S_CTLMASK,	0xFF);

	int i;
	for (i = 0; i < NR_IRQ; i++)
		irq_table[i] = spurious_irq;
}


/*****************************************************************************
 *                                put_irq_handler
 *****************************************************************************/
/**
 * <Ring 0> Set a IRQ handler.
 * 
 * @param irq     The IRQ nr.
 * @param handler The handler.
 *****************************************************************************/
PUBLIC void put_irq_handler(int irq, irq_handler handler)
{
	disable_irq(irq);
	irq_table[irq] = handler;
}


/*****************************************************************************
 *                                spurious_irq
 *****************************************************************************/
/**
 * The default IRQ handler.
 * 
 * @param irq IRQ nr.
 *****************************************************************************/
PUBLIC void spurious_irq(int irq)
{
	disp_str("<spurious_irq: ");
	disp_int(irq);
	disp_str(">\n");
}

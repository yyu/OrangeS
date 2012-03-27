/*************************************************************************//**
 *****************************************************************************
 * @file   start.c
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/*****************************************************************************
 *                                cstart
 *****************************************************************************/
/**
 * Do some preparation before `lgdt' and `lidt', then call init_prot().
 * 
 *****************************************************************************/
PUBLIC void cstart()
{
	/**
	 * Prepare the new GDT.
	 */
	memcpy(	&gdt,				   /* New GDT */
		(void*)(*((u32*)(&gdt_ptr[2]))),   /* Base  of Old GDT */
		*((u16*)(&gdt_ptr[0])) + 1	   /* Limit of Old GDT */
		);

	/**
	 * gdt_ptr[6] is used as the operand of `sgdt' and `lgdt', it
	 * contains 6 bytes (48 bits):
	 *  - bit  0~15: Limit
	 *  - bit 16~47: Base
	 */
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);

	*p_gdt_base  = (u32)&gdt;
	*p_gdt_limit = GDT_SIZE * sizeof(struct descriptor) - 1;
	/* WHY `- 1':                                       ~~~
	     The limit value for the GDT is expressed in bytes.
	   As with segments, the limit value is added to the base
	   ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	   address to get the address of the last valid byte. A
	   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	   limit value of 0 results in exactly one valid byte.
	   Because segment descriptors are always 8 bytes long,
	   the GDT limit should always be one less than an integral
	   multiple of eight (that is, 8N – 1).
	*/

	/**
	 * idt_ptr[6] is used as the operand of `sidt' and `lidt', it
	 * contains 6 bytes (48 bits):
	 *  - bit  0~15: Limit
	 *  - bit 16~47: Base
	 */
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base  = (u32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(struct gate) - 1;
	*p_idt_base  = (u32)&idt;

	/* kernel code/data base address (phys) */
	k_code_base = seg2linear(SELECTOR_KERNEL_CS);
	k_data_base = seg2linear(SELECTOR_KERNEL_DS);

	init_prot();
}

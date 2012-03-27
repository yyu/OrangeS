/*************************************************************************//**
 *****************************************************************************
 * @file   klib.c
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "config.h"
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

#include "elf.h"


/*****************************************************************************
 *                                get_boot_params
 *****************************************************************************/
/**
 * <Ring 0~1> The boot parameters have been saved by LOADER.
 *            We just read them out.
 * 
 * @param pbp  Ptr to the boot params structure
 *****************************************************************************/
PUBLIC void get_boot_params(struct boot_params * pbp)
{
	/**
	 * Boot params should have been saved at BOOT_PARAM_ADDR.
	 * @see include/load.inc boot/loader.asm boot/hdldr.asm
	 */
	int * p = (int*)BOOT_PARAM_ADDR;
	assert(p[BI_MAG] == BOOT_PARAM_MAGIC);

	pbp->mem_size = p[BI_MEM_SIZE];
	pbp->kernel_file = (unsigned char *)(p[BI_KERNEL_FILE]);

	/**
	 * the kernel file should be a ELF executable,
	 * check it's magic number
	 */
	assert(memcmp(pbp->kernel_file, ELFMAG, SELFMAG) == 0);
}


/*****************************************************************************
 *                                get_kernel_map
 *****************************************************************************/
/**
 * <Ring 0~1> Parse the kernel file, get the memory range of the kernel image.
 *
 * - The meaning of `base':	base => first_valid_byte
 * - The meaning of `limit':	base + limit => last_valid_byte
 * 
 * @param b   Memory base of kernel.
 * @param l   Memory limit of kernel.
 *****************************************************************************/
PUBLIC int get_kernel_map(unsigned int * b, unsigned int * l)
{
	struct boot_params bp;
	get_boot_params(&bp);

	Elf32_Ehdr* elf_header = (Elf32_Ehdr*)(bp.kernel_file);

	/* the kernel file should be in ELF format */
	if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0)
		return -1;

	*b = ~0;
	unsigned int t = 0;
	int i;
	for (i = 0; i < elf_header->e_shnum; i++) {
		Elf32_Shdr* section_header =
			(Elf32_Shdr*)(bp.kernel_file +
				      elf_header->e_shoff +
				      i * elf_header->e_shentsize);
		if (section_header->sh_flags & SHF_ALLOC) {
			int bottom = section_header->sh_addr;
			int top = section_header->sh_addr +
				section_header->sh_size;

			if (*b > bottom)
				*b = bottom;
			if (t < top)
				t = top;
		}
	}
	assert(*b < t);
	*l = t - *b - 1;

	return 0;
}

/**
 * @def NR_STACK_MAG_INT
 * @brief How many magic integers will be written into a proc stack.
 */
#define	NR_STACK_MAG_INT	32
/**
 * @def STACK_MAG_INT
 * @brief The magic integer.
 * I use two words including only `a' to 'f', which are also legal hex
 * numbers. I think two words are more interesting than some meaningless
 * stuff.
 *
 * Words including only `a' to `f' can be get this way:
 * - $ cat /usr/share/dict/american-english | egrep '^[a-f]+$'
 */
#define	STACK_MAG_INT		0xFadedBed
/*****************************************************************************
 *                                init_stack
 *****************************************************************************/
/**
 * Put some magic numbers into the lowest area of a proc's stack.
 * The current proc will be check to make sure these magic numbers have never
 * been changed before schedule() is invoked in clock_handler().
 * 
 * @param p  The proc whose stack is to be initialized.
 *****************************************************************************/
PUBLIC void init_stack(struct proc * p)
{
	char * va = p->stack_base - p->stack_size;
	int * la = va2la(proc2pid(p), va);
	int i;
	for (i = 0; i < NR_STACK_MAG_INT; i++)
		la[i] = STACK_MAG_INT;
}

/*****************************************************************************
 *                                check_stack
 *****************************************************************************/
/**
 * Simply check the lowest area of a proc's stack. If the bytes are not as we
 * expected, a stack overflow is considered happened.
 * 
 * @param p  The proc whose stack is to be checked.
 *****************************************************************************/
PUBLIC void check_stack(struct proc * p)
{
	char * va = p->stack_base - p->stack_size;
	int * la = va2la(proc2pid(p), va);

	if ((char*)p->regs.esp <= va)
		panic("check_stack() failed, proc:%s(%d), esp: 0x%x, eip: 0x%x"
		      " stack base: 0x%x, size: 0x%x",
		      p->name, proc2pid(p), p->regs.esp, p->regs.eip,
		      p->stack_base, p->stack_size);

	int i;
	for (i = 0; i < NR_STACK_MAG_INT; i++)
		if (la[i] != STACK_MAG_INT)
			panic("check_stack() failed, "
			      "pid: %d, esp: 0x%x, baddie: 0x%x (offset 0x%x)",
			      proc2pid(p),
			      p->regs.esp,
			      la[i],
			      i * 4 /* byte offset */
				);
}


/*****************************************************************************
 *                                disp_int
 *****************************************************************************/
/**
 * Display an integer. Used when TTY is not ready.
 * 
 * @param input The integer to be displayed.
 *****************************************************************************/
PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}


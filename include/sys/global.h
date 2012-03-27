/*************************************************************************//**
 *****************************************************************************
 * @file   global.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#else
#define	EXTERN	extern
#endif

EXTERN	int			ticks;
EXTERN	int			sync_lock;
EXTERN	int			disp_pos;

EXTERN	u8			gdt_ptr[6];	/* 0~15:Limit  16~47:Base */
/**
 * @todo To align the base addresses of the GDT on an eight-byte
 *       boundary will yield the best processor performance.
 */
EXTERN	struct descriptor	gdt[GDT_SIZE];
EXTERN	u8			idt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	struct gate		idt[IDT_SIZE];

EXTERN	u32			k_reenter;
EXTERN	u32			k_code_base;
EXTERN	u32			k_data_base;

EXTERN	struct tss		tss;
EXTERN	struct proc *		proc_ready;

EXTERN	int			current_console;

EXTERN	int			key_pressed; /**
					      * used for clock_handler
					      * to wake up TASK_TTY when
					      * a key is pressed
					      */

extern	struct proc		proc_table[];
extern	char			task_stack[];
extern	struct task		task_table[];
extern	struct task		user_proc_table[];
extern	TTY			tty_table[];
extern	CONSOLE			console_table[];
extern	irq_handler		irq_table[];
extern	system_call		sys_call_table[];

/* MM */
EXTERN	MESSAGE			mm_msg;
extern	u8 *			mmbuf;
extern	const int		MMBUF_SIZE;
EXTERN	int			memory_size;

/* FS */
EXTERN	struct file_desc	f_desc_table[NR_FILE_DESC];
EXTERN	struct inode		inode_table[NR_INODE];
EXTERN	struct super_block	super_block[NR_SUPER_BLOCK];
extern	u8 *			fsbuf;
extern	const int		FSBUF_SIZE;
EXTERN	MESSAGE			fs_msg;
EXTERN	struct proc *		pcaller;
EXTERN	struct inode *		root_inode;
extern	struct dev_drv_map	dd_map[];

/* EXTERN	int			errno; */

/* for test only */
extern	char *			logbuf;
extern	const int		LOGBUF_SIZE;
extern	char *			logdiskbuf;
extern	const int		LOGDISKBUF_SIZE;
EXTERN	struct {
	int		a;
	int		b;
	int		c;
	int		d;
} tt;

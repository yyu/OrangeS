/*************************************************************************//**
 *****************************************************************************
 * @file   proto.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2005
 *****************************************************************************
 *****************************************************************************/

PUBLIC void	panic(const char *fmt, ...);

/* lib/kliba.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disable_int();
PUBLIC void	enable_int();
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);
PUBLIC void	port_read(u16 port, void* buf, int n);
PUBLIC void	port_write(u16 port, void* buf, int n);
PUBLIC void	glitter(int row, int col);

/* kernel/protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2linear(u16 seg);
PUBLIC void	disable_irq(int irq);
PUBLIC void	enable_irq(int irq);
PUBLIC void	init_desc(struct descriptor * d, u32 base, u32 lim, u16 attr);

/* lib/klib.c */
PUBLIC void	init_stack(struct proc * p);
PUBLIC void	check_stack(struct proc * p);
PUBLIC void	get_boot_params(struct boot_params * pbp);
PUBLIC int	get_kernel_map(unsigned int * b, unsigned int * l);
PUBLIC void	disp_int(int input);
PUBLIC int	is_alphanumeric(char ch);
/* PUBLIC void	delay(int time); */
PUBLIC char *	itoa(char * str, int num);
PUBLIC void	spin(char * func_name);
PUBLIC void	print_buf(const char * bufname, const unsigned char * buf, int n);

/* kernel/kernel.asm */
PUBLIC void	restart();

/* kernel/main.c */
PUBLIC void	TestA();
PUBLIC void	TestB();
PUBLIC void	TestC();

/* kernel/init.c */
PUBLIC void	Init();

/* kernel/i8259.c */
PUBLIC void	init_8259A();
PUBLIC void	put_irq_handler(int iIRQ, irq_handler handler);
PUBLIC void	spurious_irq(int irq);

/* kernel/clock.c */
PUBLIC void	init_clock();
PUBLIC void	clock_handler(int irq);
PUBLIC void	milli_delay(int milli_sec);

/* kernel/proc.c */
PUBLIC void*	va2la(int pid, void* va);
/* PUBLIC void	wait(int lock); */
/* PUBLIC void	signal(int lock); */
PUBLIC void	schedule();
PUBLIC int	ldt_seg_linear(struct proc * p, int idx);
PUBLIC void	inform_int(int task_nr);
PUBLIC int	send_recv(int function, int src_dest, MESSAGE* msg);
PUBLIC void	reset_msg(MESSAGE* p);
PUBLIC void	dump_proc(struct proc * p, int fd);
PUBLIC void	dump_msg(const char * title, MESSAGE* p_msg);

/* kernel/hd.c */
PUBLIC void	task_hd();
PUBLIC void	hd_handler(int irq);

/* kernel/keyboard.c */
PUBLIC void	init_keyboard();
PUBLIC void	keyboard_handler(int irq);
PUBLIC void	keyboard_read(TTY* tty);

/* kernel/tty.c */
PUBLIC void	task_tty();
PUBLIC void	in_process(TTY* tty, u32 key);

/* kernel/systask.c */
PUBLIC void	task_sys();

/* kernel/console.c */
PUBLIC void	select_console(int nr_console);
PUBLIC void	init_screen(TTY* tty);
PUBLIC void	out_char(CONSOLE* con, char ch);
PUBLIC void	scroll_screen(CONSOLE* con, int dir);
PUBLIC int	is_current_console(CONSOLE* con);

/* kernel/printk.c */
PUBLIC	int	printm(const char *fmt, ...);

/* fs/main.c */
PUBLIC void			task_fs();
PUBLIC struct inode *		get_inode(int dev, int num);
PUBLIC void			put_inode(struct inode * pinode);
PUBLIC void			sync_inode(struct inode * p);
PUBLIC int			rw_sector(int io_type, int dev, u64 pos,
					  int bytes, int proc_nr, void * buf);
PUBLIC struct super_block *	get_super_block(int dev);
PUBLIC void			dump_inode_table();

/* fs/open.c */
PUBLIC int		do_open();
PUBLIC int		do_close();
PUBLIC int		do_lseek();

/* fs/read_write.c */
PUBLIC int		do_rdwt();

/* fs/link.c */
PUBLIC int		do_unlink();

/* fs/misc.c */
PUBLIC int		do_stat();
PUBLIC int		strip_path(char * filename, const char * pathname,
				   struct inode** ppinode);
PUBLIC int		search_file(char * path);

/* fs/disklog.c */
PUBLIC int		disklog(char * logstr); /* for debug */
PUBLIC void		dump_fd_graph(const char * fmt, ...);

/* mm/main.c */
PUBLIC void		task_mm();
PUBLIC int		alloc_mem(int pid, int memsize);
PUBLIC int		free_mem(int pid);

/* mm/forkexit.c */
PUBLIC int		do_fork();
PUBLIC void		do_exit(int status);
PUBLIC void		do_wait();

/* mm/exec.c */
PUBLIC int		do_exec();


/************************************************************************/
/*                       System call related below                      */
/************************************************************************/

/* kernel/proc.c */
PUBLIC int	sys_sendrec	(int function, int src_dest, MESSAGE* p_msg,
				 struct proc * p_proc);
PUBLIC int	sys_printx	(int _unused1, int _unused2, char* s,
				 struct proc * p_proc);

/* kernel/kernel.asm */
PUBLIC	void	sys_call();	/* int_handler */


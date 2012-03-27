/*************************************************************************//**
 *****************************************************************************
 * @file   stdio.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2008
 *****************************************************************************
 *****************************************************************************/

#ifndef	_ORANGES_STDIO_H_
#define	_ORANGES_STDIO_H_

#include "type.h"

/* the assert macro */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp)  if (exp) ; \
        else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

/* EXTERN */
#define	EXTERN	extern	/* EXTERN is defined as extern except in global.c */

/* string */
#define	STR_DEFAULT_LEN	1024

#define	O_CREAT		1
#define	O_RDWR		2
#define	O_TRUNC		4

#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

#define	MAX_PATH	128

/**
 * @def   MAX_FILENAME_LEN
 * @brief Max len of a filename
 * @see   dir_entry
 */
#define	MAX_FILENAME_LEN	12

/**
 * @struct dir_entry stdio.h "include/stdio.h"
 * @brief  Directory Entry
 */
struct dir_entry {
	int	inode_nr;		/**< inode nr. */
	char	name[MAX_FILENAME_LEN];	/**< Filename */
};

/**
 * @def   DIR_ENTRY_SIZE
 * @brief The size of directory entry in the device.
 *
 * It is as same as the size in memory.
 */
#define	DIR_ENTRY_SIZE	sizeof(struct dir_entry)

/**
 * @struct stat
 * @brief  File status, returned by syscall stat();
 */
struct stat {
	int st_dev;		/* major/minor device number */
	int st_ino;		/* i-node number */
	int st_mode;		/* file mode, protection bits, etc. */
	int st_rdev;		/* device ID (if special file) */
	int st_size;		/* file size */
};

/**
 * @struct time
 * @brief  RTC time from CMOS.
 */
struct time {
	u32 year;
	u32 month;
	u32 day;
	u32 hour;
	u32 minute;
	u32 second;
};

#define  BCD_TO_DEC(x)      ( (x >> 4) * 10 + (x & 0x0f) )

/*================================*
 * printf, printm, printl, printx *
 *================================*
 *
 *   printf:
 *
 *           [send msg]                WRITE           DEV_WRITE
 *                      USER_PROC ------------→ FS -------------→ TTY
 *                              ↖______________↙↖_______________/
 *           [recv msg]             SYSCALL_RET       SYSCALL_RET
 *
 *----------------------------------------------------------------------
 *
 *   printm: print via sending message to TTY (IPC)
 *
 *           [send msg]               SYS_TTY_WRITE
 *                      USER_PROC -------------------→ TTY
 *                              ↖_____________________/
 *           [recv msg]                SYSCALL_RET
 *
 *----------------------------------------------------------------------
 *
 *   printl: variant-parameter-version printx
 *
 *          calls vsprintf, then printx (trap into kernel directly)
 *
 *----------------------------------------------------------------------
 *
 *   printx: low level print without using IPC
 *
 *                       trap directly
 *           USER_PROC -- -- -- -- -- --> KERNEL
 *
 *
 *----------------------------------------------------------------------
 */

/* lib/printf.c */
PUBLIC	int	vsprintf(char *buf, const char *fmt, va_list args);
PUBLIC	int	sprintf(char *buf, const char *fmt, ...);
PUBLIC	int	printf(const char *fmt, ...); /* send message to FS */
PUBLIC	int	printl(const char *fmt, ...); /* calls `vsprintf' then `printx' */


/* kernel/syscall.asm */
PUBLIC	int	sendrec		(int function, int src_dest, MESSAGE* p_msg);
PUBLIC	int	printx		(char* str); /* a syscall, trap into the
					      * kernel directly
					      */


/*--------*/
/* 库函数 */
/*--------*/

#ifdef ENABLE_DISK_LOG
#define SYSLOG syslog
#endif

/* lib/open.c */
PUBLIC	int	open		(const char *pathname, int flags);

/* lib/lseek.c */
PUBLIC	int	lseek		(int fd, int offset, int whence);

/* lib/close.c */
PUBLIC	int	close		(int fd);

/* lib/read.c */
PUBLIC int	read		(int fd, void *buf, int count);

/* lib/write.c */
PUBLIC int	write		(int fd, const void *buf, int count);

/* lib/unlink.c */
PUBLIC	int	unlink		(const char *pathname);

/* lib/fork.c */
PUBLIC int	fork		();

/* lib/exit.c */
PUBLIC void	exit		(int status);

/* lib/wait.c */
PUBLIC int	wait		(int * status);

/* lib/exec.c */
PUBLIC int	exec		(const char * path);
PUBLIC int	execl		(const char * path, const char *arg, ...);
PUBLIC int	execv		(const char * path, char * argv[]);

/* lib/tick.c */
PUBLIC int	get_ticks();

/* lib/getpid.c */
PUBLIC int	getpid		();

/* lib/stat.c */
PUBLIC int	stat		(const char *path, struct stat *buf);

/* lib/syslog.c */
PUBLIC	int	syslog		(const char *fmt, ...);

#endif /* _ORANGES_STDIO_H_ */

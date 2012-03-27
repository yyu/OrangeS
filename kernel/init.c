/*************************************************************************//**
 *****************************************************************************
 * @file   init.c
 * @brief  Init proc, internal commands and Shabby Shell
 * @author Forrest Y. Yu
 * @date   2011
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

/**
 * @struct posix_tar_header
 * Borrowed from GNU `tar'
 */
struct posix_tar_header
{				/* byte offset */
	char name[100];		/*   0 */
	char mode[8];		/* 100 */
	char uid[8];		/* 108 */
	char gid[8];		/* 116 */
	char size[12];		/* 124 */
	char mtime[12];		/* 136 */
	char chksum[8];		/* 148 */
	char typeflag;		/* 156 */
	char linkname[100];	/* 157 */
	char magic[6];		/* 257 */
	char version[2];	/* 263 */
	char uname[32];		/* 265 */
	char gname[32];		/* 297 */
	char devmajor[8];	/* 329 */
	char devminor[8];	/* 337 */
	char prefix[155];	/* 345 */
	/* 500 */
};

/*****************************************************************************
 *                                untar
 *****************************************************************************/
/**
 * Extract the tar file and store them.
 * 
 * @param filename The tar file.
 *****************************************************************************/
PRIVATE void untar(const char * filename)
{
	printf("[extract `%s'\n", filename);
	int fd = open(filename, O_RDWR);
	assert(fd != -1);

	char buf[SECTOR_SIZE * 16];
	int chunk = sizeof(buf);
	int i = 0;
	int bytes = 0;

	while (1) {
		bytes = read(fd, buf, SECTOR_SIZE);
		assert(bytes == SECTOR_SIZE); /* size of a TAR file must be multiple of 512 */
		if (buf[0] == 0) {
			if (i == 0)
				printf("    need not unpack the file.\n");
			break;
		}
		i++;

		struct posix_tar_header * phdr = (struct posix_tar_header *)buf;

		/* calculate the file size */
		char * p = phdr->size;
		int f_len = 0;
		while (*p)
			f_len = (f_len * 8) + (*p++ - '0'); /* octal */

		int bytes_left = f_len;
		int fdout = open(phdr->name, O_CREAT | O_RDWR | O_TRUNC);
		if (fdout == -1) {
			printf("    failed to extract file: %s\n", phdr->name);
			printf(" aborted]\n");
			close(fd);
			return;
		}
		printf("    %s\n", phdr->name);
		while (bytes_left) {
			int iobytes = min(chunk, bytes_left);
			read(fd, buf,
			     ((iobytes - 1) / SECTOR_SIZE + 1) * SECTOR_SIZE);
			bytes = write(fdout, buf, iobytes);
			assert(bytes == iobytes);
			bytes_left -= iobytes;
		}
		close(fdout);
	}

	if (i) {
		lseek(fd, 0, SEEK_SET);
		buf[0] = 0;
		bytes = write(fd, buf, 1);
		assert(bytes == 1);
	}

	close(fd);

	printf(" done, %d files extracted]\n", i);
}

/*****************************************************************************
 *                                std_out
 *****************************************************************************/
/**
 * std_out  Output a string. Used before printf() is available.
 * 
 * @param s The string to be output.
 *****************************************************************************/
PRIVATE void std_out(char * s)
{
	write(1, s, strlen(s));
}

#ifdef SYSLOG
#undef SYSLOG
#define SYSLOG printl
#endif

/*****************************************************************************
 *                                internal commands
 *****************************************************************************/
PRIVATE int internal_cmd(const char * cmd)
{
	int ret = 1;

	if (strcmp(cmd, "ls0") == 0) { /* INIT_9::wait() 而后 INIT_9_11::exit() ---- INIT_9_11 无子进程 */
		int pid = fork();
		SYSLOG("shabby_shell::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent */
			int s;
			int child = wait(&s);
			SYSLOG("shabby_shell::parent::child(%d)=wait(%d);\n",
			       child, s);
			printf("\n<<< SHELL is blocked for child 0x%x\n", child);
		}
		else {	/* child */
			exec("/ls");
			SYSLOG("shabby_shell::child::exec(\"/ls\");\n");
		}
		printf("\n<<< SHELL is unblocked, for ls (PID:0x%x) was just finished.\n", getpid());
	}
	else if (strcmp(cmd, "ls1") == 0) { /* INIT::wait() 而后 INIT_9::exit() ---- INIT_9 有子进程 */
		int pid = fork();
		SYSLOG("shabby_shell::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent */
			exit(1);
		}
		else {	/* child */
			exec("/ls");
			SYSLOG("shabby_shell::child::exec(\"/ls\");\n");
		}
		printf("\n<<< SHELL is unblocked, for ls (PID:0x%x) was just finished.\n", getpid());
	}
	else if (strcmp(cmd, "ls2") == 0) { /* INIT_9_11::exit() 而后 INIT_9::wait() ---- INIT_9_11 无子进程 */
		int pid = fork();
		SYSLOG("shabby_shell::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent */
			printf("before milli_delay()\n");
			milli_delay(10000);
			printf("after  milli_delay()\n");
			int s;
			int child = wait(&s);
			SYSLOG("shabby_shell::parent::child(%d)=wait(%d);\n",
			       child, s);
			printf("\n<<< SHELL is blocked for child 0x%x\n", child);
		}
		else {	/* child */
			exec("/ls");
			SYSLOG("shabby_shell::child::exec(\"/ls\");\n");
		}
		printf("\n<<< SHELL is unblocked, for ls (PID:0x%x) was just finished.\n", getpid());
	}
	else if (strcmp(cmd, "ls3") == 0) { /* INIT_9::exit 而后 INIT::wait() ---- INIT_9 有子进程 */
		int pid = fork();
		SYSLOG("shabby_shell::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent */
			exit(1);
		}
		else {	/* child */
			exec("/ls");
			SYSLOG("shabby_shell::child::exec(\"/ls\");\n");
		}
		printf("\n<<< SHELL is unblocked, for ls (PID:0x%x) was just finished.\n", getpid());
	}
	else if (strcmp(cmd, "badwait") == 0) {
		int pid = fork();
		SYSLOG("shabby_shell::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent */
			exit(1);
		}
		else {	/* child */
			int s;
			int child = wait(&s);
			SYSLOG("shabby_shell::parent::child(%d)=wait(%d);\n",
			       child, s);
		}
		printf("\n<<< SHELL is unblocked, for ls (PID:0x%x) was just finished.\n", getpid());
	}
	else if (strcmp(cmd, "chaos") == 0) { /* FIXME: 如果两个进程同时 read(tty1) 会发生什么？ 待解决。 */
		int pid = fork();
		SYSLOG("shabby_shell::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent */
			printf("parent running\n");
		}
		else {	/* child */
			printf("child running\n");
		}
	}
	else if ((cmd[0] | 0x20) == 'k') {
		MESSAGE m;
		m.type = SYS_TEST;
		send_recv(BOTH, TASK_SYS, &m);
		printf("SYS_TEST is done\n");
	}
	else if (strcmp(cmd, "rm") == 0) {
		unlink("/cmd.tar");
	}
	else if ((strcmp(cmd, "naO") == 0) ||
		 (strcmp(cmd, "nao") == 0)) {
		std_out("naonao loves naO!\n");
	}
	else if ((strcmp(cmd, "naonao") == 0) ||
		 (strcmp(cmd, "nAoNao") == 0) ||
		 (strcmp(cmd, "NaonAo") == 0)) {
		std_out("naO loves naonao!\n");
	} else {
		ret = 0;
	}
	return ret;
}

/*****************************************************************************
 *                                shabby_shell
 *****************************************************************************/
/**
 * A very very simple shell.
 * 
 * @param tty_name  TTY file name.
 *****************************************************************************/
/* #define VERBOSE_SHELL */
PRIVATE void shabby_shell(const char * tty_name)
{
	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	SYSLOG("shabby_shell::fd_stdin(%d)=open(\"%s\", O_RDWR);\n", fd_stdin, tty_name);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);
	SYSLOG("shabby_shell::fd_stdout(%d)=open(\"%s\", O_RDWR);\n", fd_stdout, tty_name);

	char rdbuf[128];

	while (1) {
		write(1, "$ ", 2);
		int r = read(0, rdbuf, 70);
		rdbuf[r] = 0;

		if (rdbuf[0] == 0)
			continue;

		if (internal_cmd(rdbuf))
			continue;

		int argc = 0;
		char * argv[512];
		char * p = rdbuf;
		char * s;
		int word = 0;
		char ch;
		do {
			ch = *p;
			if (*p != ' ' && *p != 0 && !word) {
				s = p;
				word = 1;
			}
			if ((*p == ' ' || *p == 0) && word) {
				word = 0;
				argv[argc++] = s;
				*p = 0;
			}
			p++;
		} while(ch);
		argv[argc] = 0;

		int fd = open(argv[0], O_RDWR);
		if (fd == -1) {
			if (rdbuf[0]) {
				write(1, "{", 1);
				write(1, rdbuf, r);
				write(1, "}\n", 2);
			}
		}
		else {
			close(fd);
#ifdef VERBOSE_SHELL
			printf("{shell} will execute %s ...\n", argv[0]);
#endif
			int pid = fork();
			if (pid != 0) { /* parent */
				int s;
#ifdef VERBOSE_SHELL
				int child = wait(&s);
				printf("parent waited for 0x%x\n", child);
				printf("child pid: %d, return val: %d\n", pid, s);
#else
				wait(&s);
#endif
			}
			else {	/* child */
#ifdef VERBOSE_SHELL
				printf("{shell} --------0x%x\n", getpid());
#endif
				execv(argv[0], argv);
			}
		}
	}

	close(1);
	close(0);
}

/*****************************************************************************
 *                                Init
 *****************************************************************************/
/**
 * The hen.
 * 
 *****************************************************************************/
PUBLIC void Init()
{
	printl("Init() is running ...\n"); /* do not use printf, 'coz stdout is not ready yet */

	int fd_stdin  = open("/dev_tty0", O_RDWR);
	assert(fd_stdin  == 0);
	SYSLOG("Init::fd_stdin(%d)=open(\"/dev_tty0\", O_RDWR);\n", fd_stdin);
	int fd_stdout = open("/dev_tty0", O_RDWR);
	assert(fd_stdout == 1);
	SYSLOG("Init::stdout(%d)=open(\"/dev_tty0\", O_RDWR);\n", fd_stdout);


	/* extract `cmd.tar' */
	printf("init(): 111\n");
	untar("/cmd.tar");
	printf("init(): 222\n");
	SYSLOG("Init::untar(\"/cmd.tar\");\n");

	char * tty_list[] = {"/dev_tty1", "/dev_tty2"};

	int i;
	for (i = 0; i < sizeof(tty_list) / sizeof(tty_list[0]); i++) {
		int pid = fork();
		SYSLOG("Init::pid(%d)=fork();\n", pid);
		if (pid != 0) { /* parent process */
			printf("[parent is running, child pid:%d]\n", pid);
		}
		else {	/* child process */
			printf("[child is running, pid:%d]\n", getpid());
			close(fd_stdin);
			SYSLOG("Init::close(%d);\n", fd_stdin);
			close(fd_stdout);
			SYSLOG("Init::close(%d);\n", fd_stdout);
			
			shabby_shell(tty_list[i]);
			assert(0);
		}
	}

	while (1) {
		int s;
		int child = wait(&s);
		printf("child (%d) exited with status: %d.\n", child, s);
	}
}

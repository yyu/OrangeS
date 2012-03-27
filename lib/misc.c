/*************************************************************************//**
 *****************************************************************************
 * @file   misc.c
 * @brief  
 * @author Forrest Y. Yu
 * @date   2008
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "string.h"
#include "const.h"


/*****************************************************************************
 *                                send_recv
 *****************************************************************************/
/**
 * <Ring 1~3> IPC syscall.
 *
 * It is an encapsulation of `sendrec',
 * invoking `sendrec' directly should be avoided
 *
 * @param function  SEND, RECEIVE or BOTH
 * @param src_dest  The caller's proc_nr
 * @param msg       Pointer to the MESSAGE struct
 * 
 * @return always 0.
 *****************************************************************************/
PUBLIC int send_recv(int function, int src_dest, MESSAGE* msg)
{
	int ret = 0;

	if (function == RECEIVE)
		memset(msg, 0, sizeof(MESSAGE));

	switch (function) {
	case BOTH:
		ret = sendrec(SEND, src_dest, msg);
		if (ret == 0)
			ret = sendrec(RECEIVE, src_dest, msg);
		break;
	case SEND:
	case RECEIVE:
		ret = sendrec(function, src_dest, msg);
		break;
	default:
		assert((function == BOTH) ||
		       (function == SEND) || (function == RECEIVE));
		break;
	}

	return ret;
}

/*****************************************************************************
 *                                memcmp
 *****************************************************************************/
/**
 * Compare memory areas.
 * 
 * @param s1  The 1st area.
 * @param s2  The 2nd area.
 * @param n   The first n bytes will be compared.
 * 
 * @return  an integer less than, equal to, or greater than zero if the first
 *          n bytes of s1 is found, respectively, to be less than, to match,
 *          or  be greater than the first n bytes of s2.
 *****************************************************************************/
PUBLIC int memcmp(const void * s1, const void *s2, int n)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = (const char *)s1;
	const char * p2 = (const char *)s2;
	int i;
	for (i = 0; i < n; i++,p1++,p2++) {
		if (*p1 != *p2) {
			return (*p1 - *p2);
		}
	}
	return 0;
}

/*****************************************************************************
 *                                strcmp
 *****************************************************************************/
/**
 * Compare two strings.
 * 
 * @param s1  The 1st string.
 * @param s2  The 2nd string.
 * 
 * @return  an integer less than, equal to, or greater than zero if s1 (or the
 *          first n bytes thereof) is  found,  respectively,  to  be less than,
 *          to match, or be greater than s2.
 *****************************************************************************/
PUBLIC int strcmp(const char * s1, const char *s2)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = s1;
	const char * p2 = s2;

	for (; *p1 && *p2; p1++,p2++) {
		if (*p1 != *p2) {
			break;
		}
	}

	return (*p1 - *p2);
}

/*****************************************************************************
 *                                strcat
 *****************************************************************************/
/**
 * Concatenate two strings.
 * 
 * @param s1  The 1st string.
 * @param s2  The 2nd string.
 * 
 * @return  Ptr to the 1st string.
 *****************************************************************************/
PUBLIC char * strcat(char * s1, const char *s2)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return 0;
	}

	char * p1 = s1;
	for (; *p1; p1++) {}

	const char * p2 = s2;
	for (; *p2; p1++,p2++) {
		*p1 = *p2;
	}
	*p1 = 0;

	return s1;
}

/*****************************************************************************
 *                                spin
 *************************************************************************//**
 * Forever loop.
 * 
 * @param func_name Will be displayed to tell the user in which func the loop
 *                  is running.
 *****************************************************************************/
PUBLIC void spin(char * func_name)
{
	printl("\nspinning in %s ...\n", func_name);
	while (1) {}
}


/*****************************************************************************
 *                           assertion_failure
 *************************************************************************//**
 * Invoked by assert().
 * 
 * @param exp       The failure expression itself.
 * @param file      __FILE__
 * @param base_file __BASE_FILE__
 * @param line      __LINE__
 *****************************************************************************/
PUBLIC void assertion_failure(char *exp, char *file, char *base_file, int line)
{
	printl("%c  assert(%s) failed: file: %s, base_file: %s, ln%d",
	       MAG_CH_ASSERT,
	       exp, file, base_file, line);

	/**
	 * If assertion fails in a TASK, the system will halt before
	 * printl() returns. If it happens in a USER PROC, printl() will
	 * return like a common routine and arrive here. 
	 * @see sys_printx()
	 * 
	 * We use a forever loop to prevent the proc from going on:
	 */
	spin("assertion_failure()");
	//__asm__ __volatile__("ud2");
}

/*****************************************************************************
 *                              is_alphanumeric
 *****************************************************************************/
PUBLIC int is_alphanumeric(char ch)
{
	return ((ch >= ' ') && (ch <= '~'));
}

/*****************************************************************************
 *                              itoa
 *****************************************************************************/
PUBLIC char * itoa(char * str, int num)/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
{
	char *	p = str;
	char	ch;
	int	i;
	int	flag = 0;

/* 	*p++ = '0'; */
/* 	*p++ = 'x'; */

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = 1;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}

/*****************************************************************************
 *                                print_buf
 *************************************************************************//**
 * <Ring 1~3> Print a block of memory.
 * 
 * @param bufname Header to be printed ahead.
 * @param buf     Address of the memory.
 * @param n       How many bytes to be printed.
 *****************************************************************************/
PUBLIC void print_buf(const char * bufname, const unsigned char * buf, int n)
{
	int i;
	const int line_len = 16; /**< Change line after these bytes. */
	printl("## %s ---------\n", bufname);
	for (i = 0; i < n; i ++) {
		printl("%x", buf[i]);
		printl("%s", ((i + 1) % line_len) ? " " : "\n");
	}
	printl("\n");
}

/*======================================================================*
                               delay
 *======================================================================*/
/* PUBLIC void delay(int time) */
/* { */
/* 	int i, j, k; */
/* 	for(k=0;k<time;k++){ */
/* 		/\*for(i=0;i<10000;i++){	for Virtual PC	*\/ */
/* 		for(i=0;i<10;i++){/\*	for Bochs	*\/ */
/* 			for(j=0;j<10000;j++){} */
/* 		} */
/* 	} */
/* } */



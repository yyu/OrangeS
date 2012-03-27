/*************************************************************************//**
 *****************************************************************************
 * @file   vsprintf.c
 * @brief  vsprintf()
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
#include "keyboard.h"
#include "proto.h"

/*****************************************************************************
 *                                i2a
 *****************************************************************************/
/**
 * Convert integer to string.
 * 
 * @param val   The integer.
 * @param base  16 for hexadecimal, 10 for decimal, etc.
 * @param ps    Ptr to the string buffer.
 * 
 * @return The resulting string.
 *****************************************************************************/
PRIVATE char* i2a(int val, int base, char ** ps)
{
	int m = val % base;
	int q = val / base;
	if (q) {
		i2a(q, base, ps);
	}
	*(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

	return *ps;
}

/*****************************************************************************
 *                                vsprintf
 *****************************************************************************/
/**
 * This function is like printf(), except that: 1. it accepts a va_list argument
 * instead of a variable number of arguments; 2. it outputs to a buffer instead
 * of the stdout.
 * 
 * @param buf   The output buffer.
 * @param fmt   The format string.
 * @param args  Address of the arguments.
 *
 * @see printf()
 * 
 * @return  The number of chars outputted.
 *****************************************************************************/
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;

	va_list	p_next_arg = args;
	unsigned int	m;

	char	inner_buf[STR_DEFAULT_LEN];
	char	cs;
	int	align_nr;

	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		else {		/* a format string begins */
			align_nr = 0;
		}

		fmt++;

		if (*fmt == '%') {
			*p++ = *fmt;
			continue;
		}
		else if (*fmt == '0') {
			cs = '0';
			fmt++;
		}
		else {
			cs = ' ';
		}
		while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) {
			align_nr *= 10;
			align_nr += *fmt - '0';
			fmt++;
		}

		char * q = inner_buf;
		memset(q, 0, sizeof(inner_buf));

		switch (*fmt) {
		case 'c':
			*q++ = *((char*)p_next_arg);
			p_next_arg += 4;
			break;
		case 'x':
			m = *((unsigned int*)p_next_arg);
			assert(m >= 0);
			i2a(m, 16, &q);
			p_next_arg += 4;
			break;
		case 'd':
			m = *((int*)p_next_arg);
			if (m < 0) {
				m = m * (-1);
				*q++ = '-';
			}
			i2a(m, 10, &q);
			p_next_arg += 4;
			break;
		case 's':
			strcpy(q, (*((char**)p_next_arg)));
			q += strlen(*((char**)p_next_arg));
			p_next_arg += 4;
			break;
		default:
			break;
		}

		int k;
		for (k = 0; k < ((align_nr > strlen(inner_buf)) ? (align_nr - strlen(inner_buf)) : 0); k++) {
			*p++ = cs;
		}
		q = inner_buf;
		while (*q) {
			*p++ = *q++;
		}
	}

	*p = 0;

	return (p - buf);
}

/*****************************************************************************
 *                                sprintf
 *****************************************************************************/
/**
 * This function is like printf(), except that it outputs to a buffer instead
 * of the stdout.
 * 
 * @param buf 
 * @param fmt 
 * 
 * @return 
 *****************************************************************************/
PUBLIC int sprintf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsprintf(buf, fmt, arg);
}


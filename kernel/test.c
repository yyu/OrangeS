/*************************************************************************//**
 *****************************************************************************
 * @file   test.c
 * @brief  TestA, TestB and TestC are procs for test
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
 * @def STACK_OVERFLOW_BUF_SIZE
 */
#define STACK_OVERFLOW_BUF_SIZE  (0x4000 - 1024 - 48)
/*****************************************************************************
 *                                TestA
 *****************************************************************************/
/**
 * The first routine for test.
 * @test just for test
 *****************************************************************************/
void TestA()
{
	char badboy[STACK_OVERFLOW_BUF_SIZE];
	memset(badboy, 0xAB, STACK_OVERFLOW_BUF_SIZE);
	while(1){}
}


/*****************************************************************************
 *                                TestB
 *****************************************************************************/
/**
 * The second routine for test.
 * 
 * @test just for test
 *****************************************************************************/
void TestB()
{
	while(1){}

	assert(0);		/* never arrive here */

	spin("TestB()");
}


/*****************************************************************************
 *                                TestC
 *****************************************************************************/
/**
 * The third routine for test.
 *
 * @test just for test
 *****************************************************************************/
void TestC()
{
	while(1){}

	assert(0);		/* never arrive here */

	milli_delay(2000);
	printm(" { TestC() running ... } ");

	char buf[SECTOR_SIZE];
	memset(buf, 0xBB, SECTOR_SIZE);
	int fp = open("/blah", O_CREAT | O_RDWR);
	printm("fp:%d\n", fp);
	write(fp, "long long ago, ", 15);
	write(fp, "there was a king who loved horses", 33);
	char bufr[SECTOR_SIZE];
	memset(bufr, 0, SECTOR_SIZE);
	lseek(fp, 0, SEEK_SET);
	read(fp, bufr, 10);
	printm(bufr);
	close(fp);

	int fd_stdin  = open("/dev_tty2", O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open("/dev_tty2", O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];
	do {
		int r = read(0, rdbuf, 3);
		write(1, "<", 1);
		write(1, rdbuf, r);
		write(1, ">", 1);
	} while (rdbuf[0] != ' ');

	close(1);
	close(0);

	spin("TestC()");
}

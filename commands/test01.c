#define ORANGES

#ifdef ORANGES

#include "stdio.h"
#include "string.h"

#else

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif

#define BUF_SIZE 512
#define RW_BYTES 256
static char buf[BUF_SIZE];

static void do_test(const char * filename, int flags,
		    int open_success, const char * open_comment,
		    int rw_success, const char * rw_comment)
		    /* int del) */
		    
{
	int fd;
	int ret;

	fd = open(filename, flags);

	if (open_success) {
		assert(fd != -1);
		printf("    open(%s, %d) succeeded as we wish (%s)\n",
		       filename, flags, open_comment);
		ret = write(fd, buf, RW_BYTES);
		if (rw_success) {
			assert(ret == RW_BYTES);
			printf("       write(%d, buf, %d) succeeded as we wish (%s)\n", 
			       fd, RW_BYTES, rw_comment);
		}
		else {
			assert(ret == -1);
			printf("       write(%d, buf, %d) failed as we wish (%s)\n", 
			       fd, RW_BYTES, rw_comment);
		}
		close(fd);
	}
	else {
		assert(fd == -1);
		printf("    open(%s, %d) failed as we wish (%s)\n",
		       filename, flags, open_comment);
	}

	/* if (del) { */
	/* 	/\* delete the file *\/ */
	/* 	ret = unlink(filename); */
	/* 	assert(ret == 0); */
	/* } */
}

int main(int argc, char * argv[])
{
	printf("test01\n");

	/* initialize the buffer */
	memset(buf, 0xAB, BUF_SIZE);

	/* create a file for test */
	int fd = open("test01.f", O_CREAT | O_RDWR | O_TRUNC);
	assert(fd != -1);
	int ret = write(fd, buf, BUF_SIZE);
	assert(ret == BUF_SIZE);
	ret = close(fd);
	assert(ret == 0);

	struct fopt_rslt {
		int success;
		char* comment;
	};
	struct fopt {		/* file operation test */
		int flags;
		struct fopt_rslt r[2]; /* r[0]: file does not exist; r[1]: file exists */
	};

	/* open() */
	/* 	O_CREAT  O_RDWR  O_TRUNC   |	file doesn't exists		file exists						*/
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 1	   *			   |	create it			return -1                                               */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 2	            *		   |	return -1			open it                                                 */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 3	                    *	   |	return -1			return -1                                               */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 4	   *        *		   |	create it			return -1                                               */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 5	   *                *	   |	create it			return -1  (no O_RDWR, so it fails. @see "man 2 open")  */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 6	            *       *	   |	return -1			open it & truncate it                                   */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	/* 7	   *        *       *	   |	create it			open it & truncate it                                   */
	/* 	---------------------------|------------------------------------------------------------------------------------------- */
	struct fopt open_ft[] = {
		{O_CREAT,			{{1, "create it"},  {0, "file exists"}}},
		{O_RDWR,			{{0, "no O_CREAT"}, {1, "open it"}}},
		{O_TRUNC,			{{0, "no O_CREAT"}, {0, "file exists"}}},
		{O_CREAT | O_RDWR,		{{1, "create it"},  {0, "file exists"}}},
		{O_CREAT | O_TRUNC,		{{1, "create it"},  {0, "no O_RDWR"}}},
		{O_RDWR | O_TRUNC,		{{0, "no O_CREAT"}, {1, "open it & truncate it"}}},
		{O_CREAT | O_RDWR | O_TRUNC,	{{1, "create it"},  {1, "open it & truncate it"}}},
	};

	/* read() & write() */
	/* 	O_CREAT  O_RDWR  O_TRUNC   |	file doesn't exists		file exists		     */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 1	   *			   |	No				--                           */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 2	            *		   |	--				Yes                          */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 3	                    *	   |	--				--                           */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 4	   *        *		   |	Yes				--                           */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 5	   *                *	   |	No				--                           */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 6	            *       *	   |	--				Yes                          */
	/* 	---------------------------|---------------------------------------------------------------- */
	/* 7	   *        *       *	   |	Yes				Yes                          */
	/* 	---------------------------|---------------------------------------------------------------- */
	struct fopt rw_ft[]   = {
		{O_CREAT,			{{0, "no O_RDWR"},  {0, "NOP"}}},
		{O_RDWR,			{{0, "NOP"},        {1, "r/w it"}}},

		{O_TRUNC,			{{0, "NOP"},	    {0, "NOP"}}},
		{O_CREAT | O_RDWR,		{{1, "OK"},  	    {0, "NOP"}}},
		{O_CREAT | O_TRUNC,		{{0, "no O_RDWR"},  {0, "NOP"}}},
		{O_RDWR | O_TRUNC,		{{0, "NOP"},        {1, "OK"}}},
		{O_CREAT | O_RDWR | O_TRUNC,	{{1, "OK"},         {1, "OK"}}},
	};




	/* in the tests below, `foo' never exists(even if we create it, we delete it at once), whereas `test01.f' always exists */

	int i;
	int test_cnt = sizeof(open_ft) / sizeof(open_ft[0]);
	assert(sizeof(rw_ft) / sizeof(rw_ft[0]) == test_cnt);
	for (i = 0; i < test_cnt; i++) {
		assert(open_ft[i].flags == rw_ft[i].flags);
		printf("%da:", i+1);
		do_test("foo",      open_ft[i].flags, open_ft[i].r[0].success, open_ft[i].r[0].comment,
			rw_ft[i].r[0].success, rw_ft[i].r[0].comment);
		printf("%db:", i+1);
		do_test("test01.f", open_ft[i].flags, open_ft[i].r[1].success, open_ft[i].r[1].comment,
			rw_ft[i].r[1].success, rw_ft[i].r[1].comment);

		int r = unlink("foo");
		if (open_ft[i].r[0].success) {
			assert(r == 0);
			printf("       foo deleted.\n");
		}
		else {
			assert(r != 0);
		}
	}

	printf("\ndone\n\n");

	/* do_test(1, "foo", O_CREAT, 1, "file not exists, so create it", 0, "no O_RDWR flag", 1); */
	/* do_test(1, "test01.f", O_CREAT, 0, "file exists, cannot create", 0, "NOP", 0); */
	
	/* do_test(2, "foo", O_RDWR, 0, "no O_CREAT", 0, "NOP", 0); */
	/* do_test(2, "test01.f", O_RDWR, 1, "file exists, open it", 1, "file exists, r/w it", 0); */

	/* do_test(3, "foo", O_TRUNC, 0, "no O_CREAT", 0, "NOP", ); */
	/* do_test(3, "test01.f", O_TRUNC, 0, , 0, , ); */

	/* do_test(4, "foo", O_CREAT | O_RDWR, 1, , 0, , ); */
	/* do_test(4, "test01.f", O_CREAT | O_RDWR, 1, , 0, , ); */

	/* do_test(5, "foo", O_CREAT | O_TRUNC, 1, , 0, , ); */
	/* do_test(5, "test01.f", O_CREAT | O_TRUNC, 0, , 0, , ); */

	/* do_test(6, "foo", O_RDWR | O_TRUNC, 0, , 1, , ); */
	/* do_test(6, "test01.f", O_RDWR | O_TRUNC, 0, , 1, , ); */

	/* do_test(7, "foo", O_CREAT | O_RDWR | O_TRUNC, 1, , 1, , ); */
	/* do_test(7, "test01.f", O_CREAT | O_RDWR | O_TRUNC, 1, , 1, , ); */

	return 0;
}



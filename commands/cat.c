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
#include <unistd.h>

#endif

#define BUF_SIZE 512
static char buf[BUF_SIZE+1];

int print_file(const char * filename)
{
	int fd = open(filename, O_RDWR);
	if (fd == -1) {
		printf("failed to open file %s\n", filename);
		return -1;
	}

	int bytes;
	while ((bytes = read(fd, buf, BUF_SIZE)) > 0) {
		buf[bytes] = '\0';
		printf("%s", buf);
	}
	assert(bytes == 0 || bytes == -1);

	int ret = 0;
	if (bytes == -1) {
		printf("error reading file %s\n", filename);
		ret = -1;
	}
	close(fd);
	return ret;
}

int main(int argc, char * argv[])
{
	if (argc < 2) {
		printf("Usage: %s FILE1 FILE2 ...\n", argv[0]);
		return 0;
	}

	int i;
	for (i = 1; i < argc; i++) {
		if (print_file(argv[i]) != 0)
			break;
	}

	return 0;
}



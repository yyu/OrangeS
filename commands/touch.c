#include "stdio.h"

PUBLIC int main()
{
	write(1, "touching\n", 9);
	printf("{touch} pid:%d\n", getpid());

	return 1;
}

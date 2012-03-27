#include "stdio.h"
#include "string.h"

PUBLIC int main()
{
	/* for(;;); */
	char path[] = "/";
	struct stat s;
	int ret = stat(path, &s);
	if (ret == 0) {
		int nr_dir_entries = s.st_size / DIR_ENTRY_SIZE; /* including unused slots (the file has been deleted but the slot is still there) */
		int fd = open(path, O_RDWR);
		if (fd == -1) {
			printf("failed to open file %s\n", path);
			return -1;
		}
		struct dir_entry de;
		printf("file list of %s:\n\n", path);
		printf("  i-node                size    file name\n");
		printf(" -------    ----------------    -------------\n");
		int i;
		for (i = 0; i < nr_dir_entries; i++) {
			read(fd, &de, sizeof(de));
			if (!de.inode_nr)
				continue;
#if 1
			char fullpath[MAX_PATH];
			char filename[MAX_PATH];
			strcpy(fullpath, path);
			memcpy(filename, de.name, MAX_FILENAME_LEN);
			filename[MAX_FILENAME_LEN] = 0;
			strcat(fullpath, filename);
#else
			memcpy(fullpath + strlen(fullpath), de.name, MAX_FILENAME_LEN);
#endif
			ret = stat(fullpath, &s);
			if (ret == 0) {
				printf("%8d%12d Byte(s)    %s\n", de.inode_nr, s.st_size, filename);
			}
			else {
				printf("stat() err: %s, %d\n", fullpath, ret);
			}
		}
		printf("\n");
		close(fd);
	}
	else {
		printf("stat() err: %d\n", ret);
	}

	return 0;
}

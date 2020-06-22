#include <stdio.h>
#include <fcntl.h> // for file operations in POSIX
#include <sys/stat.h> // For permissions on file
#include "write_test.h"

int main(int argc, char* argv[]){
	int fd1 = creat("test.bin", O_SYNC | S_IRUSR | S_IWUSR);
	if(fd1 < 0){
		perror("Failed to open/create file");
		return 1;
	}

	double a = -1;

	int wr = write_test(fd1, 10000000, 100, 1, 1, &a);
	if(wr < 0){
		printf("Failed write test\n");
	}

	printf("Result is %lfs. \n", a);
}
#include <stdio.h>
#include <fcntl.h> // for file operations in POSIX
#include <unistd.h>
#include <sys/stat.h> // For permissions on file
#include "write_test.h"
#include "read_test.h"

int main(int argc, char* argv[]){
	int fd1 = open("test.bin", O_CREAT | O_RDWR | O_SYNC, 0600);
	if(fd1 < 0){
		perror("Failed to open/create file");
		return 1;
	}

	double a = -1;

	int wr = write_test(fd1, 1000000, 1000, 1, 1, &a);
	if(wr < 0){
		printf("Failed write test\n");
	}

	printf("Write result is %lfs. \n", a);

	double b = -1;

	int sr = lseek(fd1, 0, SEEK_SET);
	if(sr < 0){
		perror("Failed to seek");
		return 1;
	}
	int rr = read_test(fd1, 1000000, 1000, 1, &b);
	if(rr < 0){
		printf("Failed read test\n");
		return 1;
	}

	printf("Read result is %lfs. \n", b);

	close(fd1);
}
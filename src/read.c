#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <fcntl.h> // for file operations in POSIX
#include <stdlib.h> // for exit()
#include <unistd.h> // for close()
#include <sys/stat.h> // for fstat() (size of file)

#include "read.h"

double read_test(
	char* filename,
	unsigned long long chunk_size,
	unsigned long long chunks,
	int verbose,
	int dflag // delete file after test
){
	int file_handler = open(filename, O_SYNC | O_WRONLY);

	if(file_handler < 0){
		perror("open() failed");
		exit(1);
	}
	else if(verbose){
		printf("open() finished\n");
	}
	
	struct stat st;
	if(fstat(file_handler, &st) == -1){
		perror("fstat() failed");
		exit(1);
	}
	else if(verbose){
		printf("fstat() finished\n");
	}
	printf("Size is %ld\n", st.st_size);

	return 0.0;
}
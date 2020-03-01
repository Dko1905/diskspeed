#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <fcntl.h> // for file operations in POSIX
#include <stdlib.h> // for exit()
#include <unistd.h> // for close()
#include <sys/stat.h> // for fstat() (size of file)

#include <time.h> // for clock_gettime & timespec struct

#include "read.h"

double read_test(
	char* filename,
	unsigned long long chunk_size,
	unsigned long long chunks,
	int verbose,
	int dflag // delete file after test
){
	int file_handler = open(filename, O_SYNC | O_RDONLY);

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
	if(st.st_size < chunk_size*chunks){
		fprintf(stderr, "input file is too small\n");
		exit(2);
	}

	char* chunk = (char*)malloc(chunk_size);
	if(chunk == NULL){
		perror("malloc() failed");
		exit(1);
	}

	struct timespec start;
	struct timespec end;
	int start_result, end_result;

	start_result = clock_gettime(CLOCK_REALTIME, &start);

	for(int n = 1; n <= chunks; n++){
		if(read(file_handler, chunk, chunk_size) == -1){
			perror("read() failed");
			exit(1);
		}
	}
	
	end_result = clock_gettime(CLOCK_REALTIME, &end);
	if(end_result == -1 || start_result == -1){
		perror("clock_gettime() failed");
		exit(1);
	}
	else if(verbose){
		printf("read() finished\n");
	}
	double start_double = 1.0*start.tv_sec+1e-9*start.tv_nsec;
	double end_double = 1.0*end.tv_sec+1e-9*end.tv_nsec;
	double dif_double = end_double-start_double;

	close(file_handler);
	free(chunk);
	if(dflag){
		if(remove(filename) == -1){
			perror("Failed to remove file");
			exit(1);
		}
		else if(verbose){
			printf("removed file\n");
		}
	}

	return dif_double;
}
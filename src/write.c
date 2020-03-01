#define _POSIX_C_SOURCE 199309L

#include <sys/stat.h> // for FILE permissions (S_IRWXU)
#include <stdio.h>
#include <fcntl.h> // for file operations in POSIX
#include <stdlib.h> // for exit()
#include <unistd.h> // for close()

#include <time.h> // for clock_gettime & timespec struct

#include "write.h"

double write_test(
	char* filename,
	unsigned long long chunk_size,
	unsigned long long chunks,
	int verbose,
	int dflag // delete file after test
){
	char* chunk;

	int file_handler = open(filename, O_WRONLY | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); // Open write only and if does not exist create it (O_SYNC = minimum cache)
	if(file_handler < 0){ // if error
		perror("open() failed");
		exit(1);
	}
	else if(verbose){
		printf("open() finished\n");
	}

	chunk = (char*)malloc(chunk_size); // Allocate only one chunk and write the same chunk.
	if(chunk == NULL){
		perror("malloc() failed");
		exit(1);
	}

	struct timespec start;
	struct timespec end;
	int start_result, end_result;
	
	start_result = clock_gettime(CLOCK_REALTIME, &start);

	for(int n = 1; n <= chunks; n++){
		if(write(file_handler, chunk, chunk_size) == -1){
			perror("write() failed");
			exit(1);
		}
	}

	end_result = clock_gettime(CLOCK_REALTIME, &end);
	if(end_result == -1 || start_result == -1){
		perror("clock_gettime() failed");
		exit(1);
	}
	else if(verbose){
		printf("write() finished\n");
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
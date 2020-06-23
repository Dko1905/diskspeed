#define _POSIX_C_SOURCE 199309L
//  /\
//  | Needed for CLOCK_REALTIME to work

#include <unistd.h> // For write(2)
#include <stdio.h> // Printf
#include <errno.h> // For errno
#include <stdlib.h> // For malloc
#include <time.h> // for clock_gettime & timespec struct
#include "write.h"

#define LOG(...) \
	if(verbose_flag == 1){ \
		printf("[INFO] "); \
		printf(__VA_ARGS__); \
	}

#define ERRBUFFERSIZE 255
#define ERR_CHECK(exp, ...) \
	if(exp){ \
		char str[ERRBUFFERSIZE]; \
		snprintf(str, ERRBUFFERSIZE, __VA_ARGS__); \
		perror(str); \
		return errno; \
	}

void fill_with_random(char* buffer, size_t size);

int write_test(
	int fd,
	size_t chunk_size,
	size_t chunks,
	int verbose_flag,
	int random_flag,
	double *result
){
	char* chunk;

	chunk = (char*)malloc(chunk_size); // Allocate only one chunk and write the same chunk.
	ERR_CHECK(chunk == NULL, "malloc failed");
	LOG("Allocated %d bytes chunk\n", chunk_size);

	if(random_flag == 1){
		fill_with_random(chunk, chunk_size);
		LOG("Filled chunk with random bytes\n");
	}
	
	struct timespec start;
	struct timespec end;
	int start_result, end_result;

	start_result = clock_gettime(CLOCK_REALTIME, &start); // Start time

	int wr;
	for(size_t n = 0; n < chunks; n++){
		wr = write(fd, chunk, chunk_size);
		ERR_CHECK(wr < 1, "Failed to write"); //TODO Check if it should be `< 1` or `< 0`
	}

	end_result = clock_gettime(CLOCK_REALTIME, &end);
	ERR_CHECK(end_result < 0 || start_result < 0, "Time messurement failed");
	LOG("Done\n");

	double startd = (double)start.tv_sec + ((double)start.tv_nsec * (double)0.000000001); // I hope the compiler is good here
	double endd = (double)end.tv_sec + ((double)end.tv_nsec * (double)0.000000001);

	*result = endd - startd;

	free(chunk);
	LOG("Freed %d bytes chunk\n", chunk_size);

	return 0;
}

void fill_with_random(char* buffer, size_t size){
	for(size_t n = 0; n < size; n++){
		buffer[n] = (char)rand();
	}
}
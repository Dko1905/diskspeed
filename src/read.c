#define _POSIX_C_SOURCE 199309L
//  |
//  | Needed for CLOCK_REALTIME to work

#include <unistd.h> // For write(2)
#include <stdio.h> // Printf
#include <errno.h> // For errno
#include <stdlib.h> // For malloc
#include <time.h> // for clock_gettime & timespec struct
#include "read.h"

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

int read_test(
	int fd,
	size_t chunk_size,
	size_t chunks,
	int verbose_flag,
	double *result
){
	char* chunk;

	chunk = (char*)malloc(chunk_size); // Allocate only one chunk and write the same chunk.
	ERR_CHECK(chunk == NULL, "malloc failed");
	LOG("Allocated %lu bytes chunk\n", chunk_size);

	struct timespec start;
	struct timespec end;
	int start_result, end_result;

	start_result = clock_gettime(CLOCK_REALTIME, &start); // Start time

	int rr;
	for(size_t n = 0; n < chunks; n++){
		rr = read(fd, chunk, chunk_size);
		ERR_CHECK(rr < 1, "Failed to read");  //TODO Check if it should be `< 1` or `< 0`
	}

	end_result = clock_gettime(CLOCK_REALTIME, &end);
	ERR_CHECK(end_result < 0 || start_result < 0, "Time messurement failed");
	LOG("Done\n");

	double startd = (double)start.tv_sec + ((double)start.tv_nsec * (double)0.000000001); // I hope the compiler is good here
	double endd = (double)end.tv_sec + ((double)end.tv_nsec * (double)0.000000001);

	*result = endd - startd;

	free(chunk);
	LOG("Freed %lu bytes chunk\n", chunk_size);

	return 0;
}
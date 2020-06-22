#include <stddef.h> // For size_t

int read_test(
	int fd,
	size_t chunk_size,
	size_t chunks,
	int verbose_flag,
	double *result
);
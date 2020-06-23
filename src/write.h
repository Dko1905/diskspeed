#include <stddef.h> // For size_t

int write_test(
	int fd,
	size_t chunk_size,
	size_t chunks,
	int verbose_flag,
	int random_flag,
	double *result
);

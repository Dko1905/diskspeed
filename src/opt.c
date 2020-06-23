#pragma GCC diagnostic ignored "-Wmultichar"

#include <unistd.h> // For something //TODO check this
#include <stdio.h> // printf
#include <stdlib.h> // Standard things
#include <getopt.h> // For getopt and if gnu also getopt_long
#include <errno.h> // For errno
#include <string.h> // For strncpy
/**
 * This is commented out because we need to include them manually in the if statements
 * 
 * #include <fcntl.h> // For open() 
 * #include <sys/stat.h> // For file permissions
 * #include <sys/types.h>
 */

#include "write.h"
#include "read.h"

#define ERRBUFFERSIZE 255
#define ERR_CHECK(exp, ...) \
	if(exp){ \
		char str[ERRBUFFERSIZE]; \
		snprintf(str, ERRBUFFERSIZE, __VA_ARGS__); \
		perror(str); \
		return errno; \
	}

#define LOG(...) \
	if(verbose_flag == 1){ \
		printf("[INFO] "); \
		printf(__VA_ARGS__); \
	}

#define ERR(...) \
	fprintf(stderr, __VA_ARGS__);

void print_usage();


#ifdef __gnu_linux__
#define __USE_GNU //TODO check this

#include <fcntl.h> // For open() 
#include <sys/stat.h> // For file permissions
#include <sys/types.h>


// Check for getopt_long support

#define STRMAX 255

static struct option long_options[] = {
	{"verbose", no_argument, NULL, 'v'},
	{"write", no_argument, NULL, 'w'},
	{"read", no_argument, NULL, 'r'},
	{"delete", no_argument, NULL, 'd'},
	{"output", required_argument, NULL, 'o'},
	{"byte", required_argument, NULL, 'b'},
	{"help", no_argument, NULL, 'h'},
	{"no-random", no_argument, NULL, 'nr'}
};

int parse_and_run(int argc, char* argv[]){
	int opt = 0;
	int long_index = 0;

	// Flags
	int verbose_flag = 0;
	int write_flag = 0;
	int read_flag = 0;
	int delete_flag = 0;
	size_t chunk_size = -1, chunk_amount = -1;
	int random_flag = 0;

	char* output_filename = malloc(STRMAX);
	output_filename[STRMAX] = '\0'; // For security reasons
	ERR_CHECK(output_filename == NULL, "Failed to alocate buffers for output_filename\n");

	while(
		(opt = getopt_long(argc, argv, "vwrdo:b:", long_options, &long_index)) != -1
	){
		switch(opt){
			case 'v':
				verbose_flag = 1;
				break;
			case 'w':
				write_flag = 1;
				break;
			case 'r':
				read_flag = 1;
				break;
			case 'd':
				delete_flag = 1;
				break;
			case 'o':
				strncpy(output_filename, optarg, STRMAX-1);
				output_filename[STRMAX] = '\0'; //TODO check if I need to manually null terminate this string
				break;
			case 'b': ; // Fix bad c standard
				int sr = sscanf(optarg, "%llu:%llu", &chunk_size, &chunk_amount);
				ERR_CHECK(sr < 2, "sscanf failed\n");
				break;
			case 'h':
				print_usage();
				return 0;
				break;// This is not needed
			case 'nr':
				random_flag = 1;
			default:
				//print_usage();
				break;
		}
	}

	if(chunk_amount < 0 || chunk_size < 0){
		ERR("Invalid usage\n");
		print_usage();
		return 1;
	}

	/*
	TODO Setup opening file and read/write tests
	*/

	// File open section
	int fd = creat(output_filename, O_EXCL | S_IRUSR | S_IWUSR);
	if(fd < 0){
		if(errno == EEXIST){
			LOG("Created file successfully\n");
		}
		else{
			ERR_CHECK(1==1, "Failed to create file");
		}
	}
	close(fd);
	fd = open(output_filename, O_TRUNC | O_SYNC | O_DIRECT);
	ERR_CHECK(fd < 0, "Failed to open %s", output_filename);


	LOG("Opened %s\n", output_filename);

	int cr = close(fd);
	ERR_CHECK(cr < 0, "Failed to close %s", output_filename);

	double result = -1;

	if(write_flag){
		int wtr = write_test(fd, chunk_size, chunk_amount, verbose_flag, random_flag, &result);
	}

	free(output_filename);
	return 0;
}


#elif __unix__
// Use getopt for unix

int parse_and_run(int argc, char* argv[]){
	ERR("Not implemented\n");
	return 1;
}

#else
// Unsupported platform, say error message

int parse_and_run(int argc, char* argv[]){
	ERR("You are running on an unsupported platform!\n");
	ERR("Contact the developer if you disagree with this\n");
	return 1;
}

#endif

void print_usage(){
	printf("Usage: diskspeed -wrv -o <output file> -b <chunk size (bytes)>:<amount of chunks>\n");
}

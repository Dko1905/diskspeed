#include <unistd.h> // For something //TODO check this
#include <stdio.h> // IO/
#include <stdlib.h> // Standard things
#include <getopt.h> // For getopt and if gnu also getopt_long
#include <errno.h> // For errno
#include <string.h> // For strncpy

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
// Check for getopt_long support

#define STRMAX 255

static struct option long_options[] = {
	{"verbose", no_argument, NULL, 'v'},
	{"write", no_argument, NULL, 'w'},
	{"read", no_argument, NULL, 'r'},
	{"delete", no_argument, NULL, 'd'},
	{"output", required_argument, NULL, 'o'},
	{"byte", required_argument, NULL, 'b'}
};

int parse_and_run(int argc, char* argv[]){
	int opt = 0;
	int long_index = 0;

	// Flags
	int verbose_flag = 0;
	int write_flag = 0;
	int read_flag = 0;
	int delete_flag = 0;
	char* output_filename = malloc(STRMAX);
	size_t chunk_size = -1, chunk_amount = -1;
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
			default:
				//print_usage();
				break;
		}
	}

	/*
	TODO Setup opening file and read/write tests
	*/

	free(output_filename);
	return 0;
}


#elif __unix__
// Use getopt for unix


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
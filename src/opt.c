#include <unistd.h> // Needed for close and lseek
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
#define __USE_GNU

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
	{"no-random", no_argument, NULL, 'n'}
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
	int random_flag = 1;

	char* output_filename = malloc(STRMAX);
	output_filename[STRMAX-1] = '\0'; // For security reasons
	ERR_CHECK(output_filename == NULL, "Failed to alocate buffers for output_filename\n");

	while(
		(opt = getopt_long(argc, argv, "vwrdo:b:hn", long_options, &long_index)) != -1
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
				output_filename[STRMAX-1] = '\0'; // For security reasons
				break;
			case 'b': ; // Fix bad c standard
				int sr = sscanf(optarg, "%lu:%lu", &chunk_size, &chunk_amount);
				ERR_CHECK(sr < 2, "sscanf failed\n");
				break;
			case 'h':
				print_usage();
				return 0;
				break;// This is not needed
			case 'n':
				random_flag = 0;
				break;
			default:
				//print_usage();
				break;
		}
	}

	if(chunk_amount < 1 || chunk_size < 1){
		ERR("Invalid usage\n");
		print_usage();
		return 1;
	}
	if(!read_flag && !write_flag){
		read_flag = 1;
		write_flag = 1;
	}

	// File open section
	int fd2 = creat(output_filename, O_EXCL | S_IRUSR | S_IWUSR);
	if(fd2 < 0){
		if(errno == EEXIST){
			LOG("Created file successfully\n");
		}
		else{
			ERR_CHECK(1==1, "Failed to create file");
		}
	}
	int cr = close(fd2);
	if(cr < 0){
		perror("Failed to close file");
	}
	int fd = open(output_filename, O_TRUNC | O_SYNC | O_RDWR);
	ERR_CHECK(fd < 0, "Failed to open %s", output_filename);

	LOG("Opened %s\n", output_filename);

	double write_result = -1, read_result = -1;

	if(write_flag){
		int wtr = write_test(fd, chunk_size, chunk_amount, verbose_flag, random_flag, &write_result);
		ERR_CHECK(wtr != 0, "Failed to do write_test");
		LOG("Write test done\n");
	}
	if(read_flag){
		lseek(fd, 0, SEEK_SET); // Seek to start to read
		int rtr = read_test(fd, chunk_size, chunk_amount, verbose_flag, &read_result);
		ERR_CHECK(rtr != 0, "Failed to do read_test");
		LOG("Read test done\n");
	}
	cr = close(fd);
	ERR_CHECK(cr < 0, "Failed to close %s", output_filename);
	LOG("Closed %s\n", output_filename);

	if(delete_flag){
		int rr = remove(output_filename);
		ERR_CHECK(rr < 0, "Failed to delete %s", output_filename);
		LOG("Deleted %s\n", output_filename);
	}

	double total_data_written = (double)chunk_size * (double)chunk_amount;
	if(write_flag){
		printf("\033[94mWrite test:\033[0m\nTotal time: %fs\nTB/s: %f\nGB/s: %f\nMB/s: %f\nKB/s: %f\n", 
			write_result,
			(total_data_written/write_result)/1000000000000.0,
			(total_data_written/write_result)/1000000000.0,
			(total_data_written/write_result)/1000000.0,
			(total_data_written/write_result)/1000.0
		);
	}
	if(read_flag){
		printf("\033[94mRead test:\033[0m\nTotal time: %fs\nTB/s: %f\nGB/s: %f\nMB/s: %f\nKB/s: %f\n", 
			read_result,
			(total_data_written/read_result)/1000000000000.0,
			(total_data_written/read_result)/1000000000.0,
			(total_data_written/read_result)/1000000.0,
			(total_data_written/read_result)/1000.0
		);
	}

	free(output_filename);
	return 0;
}


#elif __unix__
// Use getopt for unix

#include <fcntl.h> // For open() 
#include <sys/stat.h> // For file permissions
#include <sys/types.h>

#define STRMAX 255

int parse_and_run(int argc, char* argv[]){
	int option;

	// Flags
	int verbose_flag = 0;
	int write_flag = 0;
	int read_flag = 0;
	int delete_flag = 0;
	size_t chunk_size = -1, chunk_amount = -1;
	int random_flag = 1;

	char* output_filename = malloc(STRMAX);
	output_filename[STRMAX-1] = '\0'; // For security reasons
	ERR_CHECK(output_filename == NULL, "Failed to alocate buffers for output_filename\n");

	while(
		(option = getopt(argc, argv, "vwrdo:b:hn")) != -1
	){
		switch(option){
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
				output_filename[STRMAX-1] = '\0'; // For security reasons
				break;
			case 'b': ; // Fix bad c standard
				int sr = sscanf(optarg, "%llu:%llu", &chunk_size, &chunk_amount);
				ERR_CHECK(sr < 2, "sscanf failed\n");
				break;
			case 'h':
				print_usage();
				return 0;
				break;// This is not needed
			case 'n':
				random_flag = 0;
				break;
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
	if(!read_flag && !write_flag){
		read_flag = 1;
		write_flag = 1;
	}

	// File open section
	int fd2 = creat(output_filename, O_EXCL | S_IRUSR | S_IWUSR);
	if(fd2 < 0){
		if(errno == EEXIST){
			LOG("Created file successfully\n");
		}
		else{
			ERR_CHECK(1, "Failed to create file");
		}
	}
	int cr = close(fd2);
	if(cr < 0){
		perror("Failed to close file");
	}
	int fd = open(output_filename, O_TRUNC | O_SYNC | O_RDWR);
	ERR_CHECK(fd < 0, "Failed to open %s", output_filename);

	LOG("Opened %s\n", output_filename);

	double write_result = -1, read_result = -1;

	if(write_flag){
		int wtr = write_test(fd, chunk_size, chunk_amount, verbose_flag, random_flag, &write_result);
		ERR_CHECK(wtr != 0, "Failed to do write_test");
		LOG("Write test done\n");
	}
	if(read_flag){
		lseek(fd, 0, SEEK_SET); // Seek to start to read
		int rtr = read_test(fd, chunk_size, chunk_amount, verbose_flag, &read_result);
		ERR_CHECK(rtr != 0, "Failed to do read_test");
		LOG("Read test done\n");
	}
	cr = close(fd);
	ERR_CHECK(cr < 0, "Failed to close %s", output_filename);
	LOG("Closed %s\n", output_filename);

	if(delete_flag){
		int rr = remove(output_filename);
		ERR_CHECK(rr < 0, "Failed to delete %s", output_filename);
		LOG("Deleted %s\n", output_filename);
	}

	double total_data_written = (double)chunk_size * (double)chunk_amount;
	if(write_flag){
		printf("\033[94mWrite test:\033[0m\nTotal time: %fs\nTB/s: %f\nGB/s: %f\nMB/s: %f\nKB/s: %f\n", 
			write_result,
			(total_data_written/write_result)/1000000000000.0,
			(total_data_written/write_result)/1000000000.0,
			(total_data_written/write_result)/1000000.0,
			(total_data_written/write_result)/1000.0
		);
	}
	if(read_flag){
		printf("\033[94mRead test:\033[0m\nTotal time: %fs\nTB/s: %f\nGB/s: %f\nMB/s: %f\nKB/s: %f\n", 
			read_result,
			(total_data_written/read_result)/1000000000000.0,
			(total_data_written/read_result)/1000000000.0,
			(total_data_written/read_result)/1000000.0,
			(total_data_written/read_result)/1000.0
		);
	}

	free(output_filename);
	return 0;
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
	printf("Usage: diskspeed --verbose --output test.bin --delete --byte 1000:1000 \n");
	printf("-v --verbose\n\tWrite extra infomation to the terminal.\n");
	printf("-r --read\n\tOnly do the read test, there is no check for file size.\n");
	printf("-d --delete\n\tDelete the file afterwords.\n");
	printf("-o --output\n\tOutput file.\n");
	printf("-b --byte\n\tSpecifies the chunk size and the amount <chunk size, chunk amount>, for example 1000:10.\n");
	printf("-h --help\n\tPrints this help message.\n");
	printf("-n --no-random\n\tDo not fill chunk with random data.\n");
	printf("\nDisclaimer: The long parameter verison only works on gnu-linux\nLicense:\tMIT\n");
}

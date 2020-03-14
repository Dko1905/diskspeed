#include <stdio.h>
#include <getopt.h>
#include <stdlib.h> // For strtoull (string to unsinged long long) & exit()
#include <string.h> // for seperating things

#include "write.h"
#include "read.h"

void print_usage();

int main(int argc, char *argv[]){
	int option;

	char* outputfile;
	int write_flag = 1;
	int read_flag = 1;
	int delete_flag = 0;
	int verbose = 0;
	unsigned long long chunk_size = 1000;
	unsigned long long chunks = 1;

	while((option = getopt(argc, argv, "o:b:hdvrw")) != -1){
		switch(option){
			case 'h': // help
				print_usage();
				exit(0);
				break;
			case 'o':
				outputfile = optarg;
				if(!outputfile){
					perror("output file is invalid.");
					exit(1);
				}
				break;
			case 'd':
				delete_flag = 1;
				break;
			case 'r':
				write_flag = 0;
				break;
			case 'w':
				read_flag = 0;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'b':
				sscanf(optarg, "%llu:%llu", &chunk_size, &chunks);
				if(chunks < 1 || chunk_size < 1){
					fprintf(stderr, "b argument is invalid\n");
					exit(1);
				}
				break;
			default:
				printf("Wrong option\n");
				print_usage();
				exit(1);
				break;
		}
	}
	double read_result = 0, write_result = 0;
	if(read_flag && !write_flag)
		read_result = read_test("test.bin", chunk_size*chunks, 1, verbose, delete_flag);
	else if(write_flag && !read_flag)
		write_result = write_test("test.bin", chunk_size, chunks, verbose, delete_flag);
	else if(write_flag && read_flag){
		write_result = write_test("test.bin", chunk_size, chunks, verbose, 0);
		read_result = read_test("test.bin", chunk_size, chunks, verbose, delete_flag);
	}
	double total_data_written = (double)chunks * (double)chunk_size;
	printf("chunk_size:%llu\nchunks:%llu\ntotal data written:%llu\n", chunk_size, chunks, chunk_size*chunks);
	if(write_result != 0){
		printf("\033[94mWrite test:\033[0m\nTotal time: %fs\nTB/s: %f\nGB/s: %f\nMB/s: %f\nKB/s: %f\n", 
			write_result,
			(total_data_written/write_result)/1000000000000.0,
			(total_data_written/write_result)/1000000000.0,
			(total_data_written/write_result)/1000000.0,
			(total_data_written/write_result)/1000.0
		);
	}
	if(read_result != 0){
		printf("\033[94mRead test:\033[0m\nTotal time: %fs\nTB/s: %f\nGB/s: %f\nMB/s: %f\nKB/s: %f\n", 
			read_result,
			(total_data_written/read_result)/1000000000000.0,
			(total_data_written/read_result)/1000000000.0,
			(total_data_written/read_result)/1000000.0,
			(total_data_written/read_result)/1000.0
		);
	}
}

void print_usage(){
	printf("Usage: diskspeed <options>\n");
	printf("\t-h\tShows help\n");
	printf("\t-o\tOutput filename\n");
	printf("\t-d\tDelete file after test(s)\n");
	printf("\t-r\tOnly read test\n");
	printf("\t-w\tOnly write test\n");
	printf("\t-v\tVerbose\n");
	printf("\t-b\tBytes -b <chunksize>:<chunks>\n");
}
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h> // For strtoull (string to unsinged long long)
#include <stdio.h> // For file operations

#include <time.h> // for the timespec struct and for some other functions. 

#define USECALLOC 0 // if you want to use calloc instad of malloc

void print_usage(){
	printf("Usage: diskspeed -o <output filename> -m <filesize in bytes>\n");
}

// dmode specifies if the test deletes files after 

double write_test(unsigned long long bytes, char* filename, int dflag, int verbose){
	// TODO Create file under a stopwatch, delete if dflag is true.
	if(verbose)
		printf("Creating/Opening %s\n", filename);
	
	FILE* fptr = fopen( filename, "w"); // open file in write mode
	// No need to seek to start, it already starts at the beginning.
	if(verbose)
	{
		printf("Created %s\n", filename);
		printf("Alocating %lld bytes of memory.\n", bytes);
	}
		


	// Create array to write to file.
	#if USECALLOC == 1
	char* bytes_to_write = (char*) calloc(bytes, sizeof(char)); // Create array filled with 0
	#else
	char* bytes_to_write = (char*) malloc(bytes); // Create array filled with random data
	#endif

	struct timespec start; // start time
	struct timespec end; // end time

	if(verbose)
		printf("Starting to write to %s\n", filename);
	clock_gettime(CLOCK_MONOTONIC, &start); // start time
	fwrite(bytes_to_write, sizeof(char), bytes, fptr);
	fflush(fptr);
	clock_gettime(CLOCK_MONOTONIC, &end); // stop time
	if(verbose)
		printf("Done writing to %s\nNow freeing buffer and closing file.\n", filename);
	free(bytes_to_write);

	fclose(fptr);
	if(dflag){ // delete file
		if(verbose)
			printf("Deleting file.\n");
		remove(filename);
	}

	// I copy pasted this
	double posix_wall = 1000.0*end.tv_sec + 1e-6*end.tv_nsec - (1000.0*start.tv_sec + 1e-6*start.tv_nsec);
	posix_wall = posix_wall / 1000;
	return posix_wall;
}
double read_test(unsigned long long bytes, char* filename, int dflag, int verbose, int create_new_file){
	FILE* fptr;
	char* bytes_to_write;
	if(create_new_file){ // If you need to create new file.
		if(verbose)
			printf("Creating/Opening %s\n", filename);
		// Open/Create file with read and write permissions.
		fptr = fopen(filename, "w+");
	}
	else{
		if(verbose)
			printf("Opening %s\n", filename);
		// Open file with read only permisions
		fptr = fopen(filename, "r");
		if(verbose)
			printf("Opened %s\n", filename);
	}

	if(verbose)
		printf("Allocating %lld bytes of memory\n", bytes);
	// Create array to write to file.
	#if USECALLOC == 1
	bytes_to_write = (char*) calloc(bytes, sizeof(char)); // Create array filled with 0
	#else
	bytes_to_write = (char*) malloc(bytes); // Create array filled with random data
	#endif


	struct timespec start; // start time
	struct timespec end; // end time

	if(create_new_file){
		if(verbose)
			printf("Writing buffer to %s\n", filename);
		fwrite(bytes_to_write, sizeof(char), bytes, fptr); // Write array to file
		fflush(fptr);
		if(verbose)
			printf("Wrote %lld bytes to %s\n", bytes, filename);
	}
	if(verbose)
		printf("Read test begins.\n");
	clock_gettime(CLOCK_MONOTONIC, &start); // start time
	fread(bytes_to_write, sizeof(char), bytes, fptr); // Read from file
	clock_gettime(CLOCK_MONOTONIC, &end); // stop time
	if(verbose)
		printf("Read test is now done.\n");
	
	// Cleanup
	free(bytes_to_write);

	fclose(fptr);
	if(dflag){ // delete file
		if(verbose)
			printf("Deleting file.\n");
		remove(filename);
	}

	// I copy pasted this
	double posix_wall = 1000.0*end.tv_sec + 1e-6*end.tv_nsec - (1000.0*start.tv_sec + 1e-6*start.tv_nsec);
	posix_wall = posix_wall / 1000;
	return posix_wall;
}

int main(int argc, char *argv[]){
	int option;
	int oflag = 0; // Output file flag
	char* output_file = "temp.bin"; // Output file
	int mflag = 0; // filesize in bytes (m is for memory)
	unsigned long long filesize = 10LL; // filesize in bytes
	int wflag = 1; // write test
	int rflag = 1; // read test
	int dfalg = 0; // dflag specifies if the test deletes files after 
	int vflag = 0; // vflag specifies if it should be verbose.
	while((option = getopt(argc, argv, "o:m:hwrdv")) != -1){
		// Check witch option was used.
		switch(option){
			case 'h': // help
				print_usage();
				exit(0);
				break;
			case 'o': // Output_file
				if(oflag){ // If flag is already set print usage.
					print_usage();
					exit(1);
				}
				else{
					oflag = 1;
					output_file = optarg;
				}
				break;
			case 'm': // specify filesize
				if(mflag){ // If flag is already set print usage.
					print_usage();
					exit(1);
				}
				else{
					mflag = 1;
					filesize = strtoull(optarg, NULL, 10); // http://www.cplusplus.com/reference/cstdlib/strtoull/
				}
				break;
			case 'w': // write mode
				// Only do write test.
				rflag = 0;
				break;
			case 'r': // read mode
				// Only do read test.
				wflag = 0;
				break;
			case 'd': // Delete files after test(s)
				dfalg = 1;
				break;
			case 'v':
				vflag = 1;
				break;
			default:
				printf("Wrong option\n");
				print_usage();
				exit(3);
				break;
		}
	}
	int file_exists = 0;
	if(wflag){
		if(!dfalg){// If you don't delete the file
			file_exists = 1; 
		}
		double time_passed = write_test(filesize,output_file, dfalg, vflag); // Get time passed
		// Print the time passed 
		printf("\nWrite test:\nTime taken: %fs\nTB/s : %f\nGB/s : %f\nMB/s : %f\nKB/s : %f\n\n", time_passed, ((double)filesize/1000000000000.0)/time_passed, ((double)filesize/1000000000.0)/time_passed, ((double)filesize/1000000.0)/time_passed, ((double)filesize/1000.0)/time_passed);
	}
	if(rflag){
		double time_passed = read_test(filesize, output_file, dfalg, vflag, !file_exists);
		printf("\nRead test:\nTime taken: %fs\nTB/s : %f\nGB/s : %f\nMB/s : %f\nKB/s : %f\n\n", time_passed, ((double)filesize/1000000000000.0)/time_passed, ((double)filesize/1000000000.0)/time_passed, ((double)filesize/1000000.0)/time_passed, ((double)filesize/1000.0)/time_passed);
	}
	if(!rflag && !wflag){
		printf("No options chosen.\n");
		print_usage();
		exit(4);
	}
	return 0;// All is okay
}

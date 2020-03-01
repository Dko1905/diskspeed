#include <stdio.h>
#include <getopt.h>
#include <stdlib.h> // For strtoull (string to unsinged long long) & exit()

#include "write.h"
#include "read.h"

void print_usage();

int main(int argc, char *argv[]){
	/*
	int option;
	while((option = getopt(argc, argv, "o:m:hwrdv")) != -1){
		switch(option){
			case 'h': // help
				print_usage();
				exit(0);
				break;
			default:
				printf("Wrong option\n");
				print_usage();
				exit(1);
				break;
		}
	}
	*/
	write_test("test.bin", 1000, 10, 1, 0);
	read_test("test.bin", 1000, 10, 1, 0);
}

void print_usage(){
	printf("Usage: diskspeed -o <output filename> -m <filesize in bytes>\n");
}
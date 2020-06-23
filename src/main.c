#include <stdio.h>
#include <fcntl.h> // for file operations in POSIX
#include <unistd.h>
#include <sys/stat.h> // For permissions on file
#include "opt.h"

int main(int argc, char* argv[]){
	return parse_and_run(argc, argv);
}
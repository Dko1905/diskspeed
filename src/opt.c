#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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

#ifdef __gnu_linux__
// Check for getopt_long support


#elif __unix__



#else
// Unsupported platform, say error message

int parse_and_run(int argc, char* argv[]){
	ERR("You are running on an unsupported platform!\n");
	ERR("Contact the developer if you disagree with this\n");
	return 1;
}

#endif
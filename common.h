#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <portaudio.h>

#define log(...) \
	do { \
		printf(__VA_ARGS__); \
		puts(""); \
		fflush(stdout); \
	} while (0)

#define dieif(cond, ...) \
	if ((cond)) { \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		exit(EXIT_FAILURE); \
	}

#define errif(cond, msg) \
	if ((cond)) { \
		perror(msg); \
		exit(EXIT_FAILURE); \
	}

#define perrorif(cond, msg) \
	if ((cond)) { \
		perror(msg); \
	}

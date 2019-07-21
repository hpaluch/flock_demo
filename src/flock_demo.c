/*
 * flock_demo.c - demonstrate potential pitfalls using flock(2) on QNX Neutrino RTOS 7 filesystem
 */
#include <stdio.h>
#include <stdlib.h>

// for errno and strerror(3)
#include <errno.h>
#include <string.h>

// sleep(3)
#include <unistd.h>

// for open
#include <sys/types.h>
#include <sys/stat.h>
// for open and flock(2)
#include <fcntl.h>
// for flock(2) on Linux
#include <sys/file.h>

#include <time.h>

#define USE_TIMESTAMP_NS
#ifdef USE_TIMESTAMP_NS
static void flock_demo_print_timestamp(void){
	struct timespec t;

	if (clock_gettime(CLOCK_MONOTONIC,&t)){
		perror("clock_gettime");
		return;
	}
	// print timestamp in seconds with ns resolution after decimal point
	printf("%llu.%09lu ", (unsigned long long)t.tv_sec, t.tv_nsec );
}
#else
static void flock_demo_print_timestamp(void){
	time_t t;
	struct tm *t2 = NULL;
	char buf[32] = { '\0' };

	t = time(NULL);
    t2 = localtime(&t);
    if (t2 == NULL){
    	perror("localtime");
    	return;
    }
    if (strftime(buf,sizeof(buf),"%H:%M:%S ",t2) == 0){
    	perror("strftime");
    	return;
    }
    // prints current time as HH:MM:SS
    fputs(buf,stdout);
}
#endif

#define FLOCK_DEMO_LOCK_MESSAGE(desc) \
		flock_demo_print_timestamp(); \
	printf("%s LOCK_EX on fd=%d, fname='%s'\n", desc, fd, lock_file);


int main(int argc, char **argv) {
	int fd = -1;
	char *lock_file = NULL;
	int  sleep_seconds = 10;
	int  ret = EXIT_FAILURE;
	unsigned to_sleep = 0;

	if (argc < 2 || argc > 3 ){
		fprintf(stderr,"Invalid count of arguments\n");
		fprintf(stderr,"Usage: %s <lock_file_path> [ sleep_seconds ]\n", argv[0]);
		fprintf(stderr,"Default sleep_seconds is = %d\n", sleep_seconds);
		goto exit0;
	}

	lock_file = argv[1];

	if (argc == 3){
		sleep_seconds = atoi(argv[2]);
		// TODO: catch also arguments like '10abc' (will silently convert to 10)
		if (sleep_seconds <= 0){
			fprintf(stderr,"Invalid sleep_seconds specified '%s'-> %d. Must be > 0\n",
					argv[2], sleep_seconds);
			goto exit0;
		}
	}

	fd = open(lock_file,O_RDWR|O_CREAT,0600);
	if (fd == -1){
		fprintf(stderr,"open('%s',...): %s\n", lock_file, strerror(errno));
		goto exit0;
	}

	FLOCK_DEMO_LOCK_MESSAGE("Acquiring");
	if (flock(fd, LOCK_EX)){
		fprintf(stderr,"flock(fd=%d, fname='%s', LOCK_EX): %s\n", fd, lock_file, strerror(errno));
		goto exit1;
	}

	FLOCK_DEMO_LOCK_MESSAGE("Acquired");
	flock_demo_print_timestamp();
	printf("Sleeping for %d seconds...\n", sleep_seconds);

	to_sleep = sleep(sleep_seconds);
	while (to_sleep > 0 ){
		printf("WARNING! Sleep returned prematurely - sleeping again for %u seconds\n", to_sleep);
		to_sleep = sleep(to_sleep);
	}

	FLOCK_DEMO_LOCK_MESSAGE("Releasing");
	if (flock(fd, LOCK_UN)){
		fprintf(stderr,"flock(fd=%d, fname='%s', LOCK_UN): %s\n", fd, lock_file, strerror(errno));
		goto exit1;
	}
	FLOCK_DEMO_LOCK_MESSAGE("Released");

	ret = EXIT_SUCCESS;
exit1:
	if (close(fd)){
		fprintf(stderr,"close(fd=%d, file='%s'): %s\n", fd, lock_file, strerror(errno));
		ret = EXIT_FAILURE;
	}
exit0:
	// NOTE: We do NOT unlink(2) lock file to reduce likehood of race conditions on create/unlink
	return ret;
}

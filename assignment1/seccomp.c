#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <linux/filter.h>
#include <linux/seccomp.h>

static int
seccomp(unsigned int operation, unsigned int flags, void *args)
{
	return syscall(__NR_seccomp, operation, flags, args);
}

int
main(int argc, char **argv)
{
	int fd;

	printf("1. before seccomp\n");
	if (seccomp(SECCOMP_SET_MODE_STRICT, 0, NULL))
		err(1, "seccomp");

	printf("2. after seccomp, trying open\n");
	fd = open("/tmp", O_RDONLY);

	printf("3. after malloc, trying close\n");
	close(fd);
	
	printf("4. exiting\n");

	return (0);
}

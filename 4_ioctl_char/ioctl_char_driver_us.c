#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/driver0"
#define IOCTL_RESET_COUNTER _IO('q', 1)

int main()
{
	int fd = open("/dev/driver0", O_RDWR);

	if(fd < 0)
	{
		printf("Cannot open device\n");
		return -1;
	}

	printf("Sending IOCTL to reset counter...\n");

	// Sending IOCTL command to kernel driver
	ioctl(fd, IOCTL_RESET_COUNTER);

	printf("IOCTL command sent\n");

	close(fd);

	return 0;
}

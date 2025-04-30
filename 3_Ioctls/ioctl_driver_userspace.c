#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVCH_IOC_MAGIC  'k'
#define DEVCH_IOCRESET    _IO(DEVCH_IOC_MAGIC, 0)
#define DEVCH_IOCSET      _IOW(DEVCH_IOC_MAGIC, 1, int)
#define DEVCH_IOCGET      _IOR(DEVCH_IOC_MAGIC, 2, int)
#define DEVCH_IOCSETGET   _IOWR(DEVCH_IOC_MAGIC, 3, int)

int main() {
    int fd, ret, value;

    // Open the device
    fd = open("/dev/DEVCHdevice", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    // IOCTL RESET (no data exchange)
    ret = ioctl(fd, DEVCH_IOCRESET);
    if (ret == -1) {
        perror("Failed IOCTL RESET");
    }

    // IOCTL SET (sending a value to kernel)
    value = 123;
    ret = ioctl(fd, DEVCH_IOCSET, &value);
    if (ret == -1) {
        perror("Failed IOCTL SET");
    }

    // IOCTL GET (getting a value from kernel)
    ret = ioctl(fd, DEVCH_IOCGET, &value);
    if (ret == -1) {
        perror("Failed IOCTL GET");
    } else {
        printf("IOCTL GET value: %d\n", value);
    }

    // IOCTL SETGET (sending and receiving a value)
    value = 50;
    ret = ioctl(fd, DEVCH_IOCSETGET, &value);
    if (ret == -1) {
        perror("Failed IOCTL SETGET");
    } else {
        printf("IOCTL SETGET modified value: %d\n", value);
    }

    // Close the device
    close(fd);
    return 0;
}


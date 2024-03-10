#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#define BUFSIZE 2000
#define TUN_NAME "crefl0"

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

int main() {
    char tun_name[IFNAMSIZ];
    char buf[BUFSIZE];
    int tun_fd, nbytes;

    strcpy(tun_name, TUN_NAME);

    if ((tun_fd = tun_alloc(tun_name)) < 0) {
        fprintf(stderr, "Error creating TUN device\n");
        exit(1);
    }

    printf("TUN device %s created\n", tun_name);

    while (1) {
        nbytes = read(tun_fd, buf, sizeof(buf));
        if (nbytes < 0) {
            perror("Reading from interface");
            close(tun_fd);
            exit(1);
        }
        printf("Read %d bytes from TUN device\n", nbytes);

        // Simply write the received packet back to the TUN device
        if (write(tun_fd, buf, nbytes) < 0) {
            perror("Writing to interface");
            close(tun_fd);
            exit(1);
        }
        printf("Sent %d bytes to TUN device\n", nbytes);
    }
    return 0;
}

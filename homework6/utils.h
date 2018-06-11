#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl : get");
        exit(0);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl : set");
        exit(0);
    }
}
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "utils.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
        fprintf(stderr, "Incorrect arguments need address to connect\n");
        return 0;
    }

    printf("Send \"exit\" to stop server\n");
    printf("Send \"disconnect\" to disconnect\n");

    const int port = 8888;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return 0;
    }

    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        perror("epoll");
        return 0;
    }

    int N = 10;
    struct epoll_event ev, events[N];
    ev.events = EPOLLIN;
    ev.data.fd = fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl");
        return 0;
    }

    ev.events = EPOLLIN;
    ev.data.fd = 1;
    non_blocking(1);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 1, &ev) == -1) {
        perror("epoll_ctl");
        return 0;
    }

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    	perror("connect");
        return 0;
    }

    int work = 1;
    while (work) {
    	int nfds = epoll_wait(epollfd, events, N, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            return 0;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == fd) {
                char response[1024];
                if (recv(fd, response, 1024, 0) == -1) {
                    perror("recv");
                    return 1;
                }       
                printf("%s : from server\n", response);
            }
            else {
                char msg[1024];
                scanf("%s", msg);
                if (send(fd, msg, strlen(msg) + 1, 0) == -1) {
                    perror("send");
                    return 0;
                }  
                if (strcmp(msg, "exit") == 0 || strcmp(msg, "disconnect") == 0) {
                    printf("%s\n", msg);
                    work = 0;
                }
            }
        }
	}

    close(fd);

	return 0;
}

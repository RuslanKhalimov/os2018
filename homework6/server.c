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

void add_fd(int server_fd, int epollfd, struct epoll_event *ev) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    printf("Connected : %d\n", client_fd);
    non_blocking(client_fd);

    ev->events = EPOLLIN;
    ev->data.fd = client_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, ev) == -1) {
        perror("epoll_ctl");
        return;
    }
}

int read_data(int fd, int epollfd, struct epoll_event *ev) {
    char msg[1024];
    int sz;
    if ((sz = recv(fd, msg, 1024, 0)) == -1) {
        perror("recv");
        return 1;
    }
    if (sz == 0 || strcmp(msg, "disconnect") == 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, ev);
        printf("Disconnected : %d\n", fd);
        close(fd);
        return 1;
    }
    printf("%s : %d\n", msg, fd);
    if (send(fd, msg, strlen(msg) + 1, 0) == -1) {
        perror("send");
        return 1;
    }
    if (strcmp(msg, "exit") == 0) {
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments need addres to bind\n");
        return 0;
    }

    const int port = 8888;

	struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 0;
    }

    non_blocking(server_fd);

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind");
        return 0;
    }

    if (listen(server_fd, 50) == -1) {
        perror("listen");
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
    ev.data.fd = server_fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl");
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
            if (events[i].data.fd == server_fd) {
                add_fd(server_fd, epollfd, &ev);
            }
            else {
                work = read_data(events[i].data.fd, epollfd, &ev);
            }
        }
    }

    close(server_fd);

	return 0;
}

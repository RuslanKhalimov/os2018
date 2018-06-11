#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "utils.h"

void read_request(int fd, char *file_name) {
    char block = 1;
    int file_fd;
    char *respond = malloc(512);
    if ((file_fd = open(file_name, O_RDONLY)) == -1) {
        respond[0] = '0';
        respond[1] = '5';
        respond[2] = '0';
        respond[3] = errno == EACCES ? '2' : '1';
        respond[4] = '\0';
        respond = strcat(respond, errno == EACCES ? "Access violation" : "File not found");
        safe_send(fd, respond);
        free(respond);
        return;
    }
    char *buf = malloc(512);

    while (1) {
        int n;
        if ((n = read(file_fd, buf, 508)) == -1) {
            error("read");
        }
        if (n == 0) {
            break;
        }

        respond[0] = '0';
        respond[1] = '3';
        respond[2] = block;
        respond[3] = '#';
        respond[4] = '\0';
        respond = strcat(respond, buf);
        
        while (1) {
            if (write(fd, respond, n + 4) == -1) {
                error("write");
            }

            int flag = wait_ack(fd, block);
            if (flag == 2) {
                free(buf);
                free(respond);
                return;
            }
            if (flag == 1) {
                ++block;
                break;
            }
        }
    }

    free(buf);
    free(respond);
    close(file_fd);
}

void write_request(int fd, char *file_name) {
    int file_fd;
    if ((file_fd = open(file_name, O_CREAT | O_WRONLY| O_EXCL)) == -1) {
        char *respond = malloc(512);
        respond[0] = '0';
        respond[1] = '5';
        respond[2] = '0';
        respond[3] = errno == EEXIST ? '6' : '0';
        respond[4] = '\0';
        respond = strcat(respond, errno == EEXIST ? "File already exists" : "open error");
        safe_send(fd, respond);
        free(respond);
        return;
    }
    char *buf = malloc(512);
    send_ack(fd, 1);

    while (1) {
        int n = safe_recv(fd, buf);
        if (n == 0) {
            break;
        }

        if (write(file_fd, buf + 4, n - 4) == -1) {
            error("write");
        }

        send_ack(fd, buf[2]);
    }

    free(buf);
    close(file_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments need address to bind\n");
        return 0;
    }

    const int port = 8888;

	struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        error("socket");
    }

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        error("bind");
    }

    if (listen(server_fd, 50) == -1) {
        error("listen");
    }

    int work = 1;
    while (work) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len);
        if (client_fd == -1) {
            error("accept");
        }

        printf("Connected\n");

        char *msg = malloc(128);
        safe_recv(client_fd, msg);

        if (msg[1] == '1') {
            read_request(client_fd, msg + 2);
        }
        else if (msg[1] == '2') {
            write_request(client_fd, msg + 2);
        }

        printf("Disconnected\n");

        close(client_fd);
    }

    close(server_fd);

	return 0;
}

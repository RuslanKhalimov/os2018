#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

void error(char *msg) {
	perror(msg);
	exit(1);
}

void safe_send(int fd, char *msg) {
    if (send(fd, msg, strlen(msg) + 1, 0) == -1) {
        error("send");
    }
}

int safe_recv(int fd, char *msg) {
    int x = recv(fd, msg, 512, 0);
    if (x == -1) {
        error("recv");
    }
    return x;
}

void send_ack(int fd, char block) {
	char ack[8];
	ack[0] = '0';
	ack[1] = '4';
	ack[2] = block;
	ack[3] = '#';
	ack[4] = '\0';
	safe_send(fd, ack);
}

int wait_ack(int fd, char block) {
    char ack[512];
    if (safe_recv(fd, ack) == 0) {
        printf("recv error\n");
        return 2;
    }
    if (strlen(ack) == 4 && ack[1] == '4' && ack[2] == block) {
        return 1;
    }
    if (strlen(ack) > 1 && ack[1] == '5') {
    	printf("%s\n", (ack + 4));
    	return 2;
    }
    return 0;
}

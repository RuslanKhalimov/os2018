#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

void read_request(int fd, char * file_name) {
	char *request = malloc(512);
	request[0] = '0';
	request[1] = '1';
	request[2] = '\0';
	request = strcat(request, file_name);

	safe_send(fd, request);

	char *data = malloc(512);
	int n = 512;
	while (n == 512) {
		if ((n = read(fd, data, 512)) == -1) {
			error("read");
		}
		if (data[1] == '5') {
			printf("%s\n", (data + 4));
			break;
		}
		else if (data[1] == '3') {
			if (write(1, data + 4, n - 4) == -1) {
				error("write");
			}
			send_ack(fd, data[2]);
		}
		else {
			send_ack(fd, 0);
		}
	}

	free(data);
	free(request);
}

void write_request(int fd, char *file_name) {
	char *request = malloc(512);
	request[0] = '0';
	request[1] = '2';
	request[2] = '\0';
	request = strcat(request, file_name);

	while (1) {
		safe_send(fd, request);
		int flag = wait_ack(fd, 1);
		if (flag == 2) {
			free(request);
			return;
		}
		if (flag == 1) {
			break;
		}
	}

	char *data = malloc(512);
	int n = 512, block = 1;
	while (n == 512) {
		if ((n = read(0, data, 508)) == -1) {
			error("read");
		}
		request[1] = '3';
		request[2] = block;
		request[3] = '#';
		request[4] = '\0';
		request = strcat(request, data);

		while (1) {
			if (write(fd, request, n + 4) == -1) {
				error("write");
			}
			int flag = wait_ack(fd, block);
			if (flag == 2) {
				free(request);
				free(data);
				return;
			}
			if (flag == 1) {
				++block;
				break;
			}
		}
	}
	free(request);
	free(data);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
        printf("Incorrect arguments need address to connect\n");
        return 0;
    }

    printf("RRQ for read request, WRQ for write request\n");
   
    const int port = 8888;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        error("socket");
    }

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    	error("connect");
    }

 	char operation[128];
 	scanf("%s", operation);

 	printf("Enter file name\n");
 	char file_name[128];
	scanf("%s", file_name);

 	if (strcmp(operation, "RRQ") == 0) {
 		read_request(fd, file_name);
 	}
 	else if (strcmp(operation, "WRQ") == 0) {
 		write_request(fd, file_name);
 	}
 	else {
 		printf("Unsupported operation\n");
 	}

    close(fd);

	return 0;
}

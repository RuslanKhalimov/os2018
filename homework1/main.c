#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int main() {
	while (1) {
		printf(">> ");
		int size = 256, len = 256;
		char *next = malloc(sizeof(char)*len);
		char **words = malloc(sizeof(char*)*size);
		int index = 0, i = 0;
		char c;
		while (c = getchar()) {
			if (c == ' ' || c == '\t' || c == '\n') {
				if (i == 0 && c != '\n') continue;
				if (i == 0 && c == '\n') break;
				words[index++] = next;
				if (c == '\n') break;
				i = 0;
				next = malloc(sizeof(char)*len);
			}
			else {
				next[i++] = c;
				if (c == EOF) {
					printf("\n");
					return 0;
				}
			}	
		}
		if (index == 0) {
			continue;
		}
		if (strcmp(words[0], "exit") == 0) {
			return 0;
		}
		words[index] = NULL;

		pid_t pid = fork();
		if (pid == 0) {
			execvp(words[0], words);
			perror("execvp");
			exit(errno);
		}
		else if (pid > 0) {
			int status;
			waitpid(pid, &status, 0);
			printf("Status : %d\n", status);
		}
		else {
			perror("fork");
			exit(errno);
		}
	}

	return 0;
}

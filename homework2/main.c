#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int inum = -1;
char *name = NULL;
int size = -1;
int cmp_size;
int nlinks = -1;
char *exec = NULL;

int check(struct dirent *next, struct stat st) {
	if (inum != -1 && next->d_ino != inum) {
		return 0;
	}
	if (name && strcmp(next->d_name, name)) {
		return 0;
	}
	if (size != -1) {
		switch (cmp_size) {
			case -1 :
				if (st.st_size >= size) {
					return 0;
				}
				break;
			case 0 :
				if (st.st_size != size) {
					return 0;
				}
				break;
			case 1 :
				if (st.st_size <= size) {
					return 0;
				}
				break;
		}
	}
	if (nlinks != -1 && nlinks != st.st_nlink) {
		return 0;
	}
	return 1;
}

void run(char *name, char *file) {
	char **args = malloc(sizeof(char*)*3);
	args[0] = name;
	args[1] = file;
	args[2] = NULL;
	pid_t pid = fork();
	if (pid == 0) {
		execvp(name, args);
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

void find(char *dirName) {
	DIR *curDir = opendir(dirName);
	struct dirent *next;
	struct stat st;
	char *path_name = malloc(sizeof(char)*strlen(dirName));
	while ((next = readdir(curDir)) != NULL) {
		switch(next->d_type) {
		    case DT_REG :
		    	path_name = strcpy(path_name, dirName);
		    	path_name = strcat(strcat(path_name, "/"), next->d_name);
		    	if (stat(path_name, &st) < 0) {
		    		perror("stat");
		    		exit(errno);
		    	}
		    	if (check(next, st)) {
		    		printf("%s\n", path_name);
		    		if (exec != NULL) {
		    			run(exec, path_name);
		    		}
		    	}	
		    	break;
		    case DT_DIR :
		    	if (strcmp(next->d_name, ".") != 0 && strcmp(next->d_name, "..") != 0) {
		    		find(next->d_name);
		    	}
				break;
			default :
				perror("d_type");
				exit(errno);
		}
	}
}

int main(int argc, char *argv[]) {
	for (int i = 2; i < argc; i += 2) {
		if (strcmp(argv[i], "-inum") == 0) {
			inum = atoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "-name") == 0) {
			name = argv[i + 1];
		}
		if (strcmp(argv[i], "-size") == 0) {
			switch(argv[i + 1][0]) {
				case '-' :
					cmp_size = -1;
					break;
				case '=' :
					cmp_size = 0;
					break;
				case '+' :
					cmp_size = 1;
					break;
			}
			size = atoi(argv[i + 1] + 1);
		}
		if (strcmp(argv[i], "-nlinks") == 0) {
			nlinks = atoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "-exec") == 0) {
			exec = argv[i + 1];
		}
	}
	find(argv[1]);

	return 0;
}
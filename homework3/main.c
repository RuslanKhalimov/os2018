#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char code[] = {0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xfc, 0x8b, 0x45, 0xfc, 0x83, 0xc0, 0x05, 0x5d, 0xc3};

	void *map_result = mmap(NULL, sizeof(code), PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (map_result == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	memcpy(map_result, code, sizeof(code));

	if (mprotect(map_result, sizeof(code), PROT_READ | PROT_EXEC) < 0) {
		perror("mprotect");
		return 1;
	}

	int x;
	scanf("%d", &x);
	printf("adding 5\n");
	printf("%d\n", ((int(*)(int))map_result)(x));

	printf("changing code\n");
	if (mprotect(map_result, sizeof(code), PROT_WRITE) < 0) {
		perror("mprotect");
		return 1;
	}
	printf("adding10\n");
	((char*)map_result)[12] = 10;
	if (mprotect(map_result, sizeof(code), PROT_READ | PROT_EXEC) < 0) {
		perror("mprotect");
		return 1;
	}
	printf("%d\n", ((int(*)(int))map_result)(x));

	if (munmap(map_result, sizeof(code)) == -1) {
		perror("munmap");
		return 1;
	}

	return 0;
}

#include <dlfcn.h>
#include <stdio.h>

int* input(int n);

int* sort(int *a, int n);

int main() {
	int n;
	scanf("%d", &n);
	int *a = input(n);
	a = sort(a, n);

	void *handle;
	void (*output)(int*, int);
	char *error;
	handle = dlopen("./liboutput.so", RTLD_LAZY);
	if (!handle) {
		fputs(dlerror(), stderr);
	}
	output = dlsym(handle, "output");
	(*output)(a, n);
	dlclose(handle);

	return 0;
}
#include <stdio.h>
#include <stdlib.h>

int* input(int n) {
	int *a = malloc(sizeof(int)*n);
	for (int i = 0; i < n; ++i) {
		scanf("%d", &a[i]);
	}
	return a;
}
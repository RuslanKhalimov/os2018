#include <stdio.h>

void output(int *a, int n) {
	for (int i = 0; i < n; ++i) {
		printf("%d ", a[i]);
	}
	printf("\n");
}
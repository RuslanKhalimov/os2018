#include <stdlib.h>

int* merge(int *a, int *b, int n, int m) {
	int i = 0, j = 0;
	int *res = malloc(sizeof(int)*(n + m));
	while (i < n && j < m) {
		if (a[i] < b[j]) {
			res[i + j] = a[i++];
		}
		else {
			res[i + j] = b[j++];
		}
	}
	while (i < n) {
		res[i + j] = a[i++];
	}
	while(j < m) {
		res[i + j] = b[j++];
	}
	return res;
}

int* sort(int *a, int n) {
	if (n == 1) {
		return a;
	}
	int m = n / 2;
	n -= m;
	int *q = malloc(sizeof(int)*n);
	int *w = malloc(sizeof(int)*m);
	for (int i = 0; i < n; ++i) {
		q[i] = a[i];
	}
	for (int i = 0; i < m; ++i) {
		w[i] = a[i + n];
	}
	q = sort(q, n);
	w = sort(w, m);
	return merge(q, w, n, m);
}
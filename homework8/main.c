#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ucontext.h>
#include <execinfo.h>
#include <signal.h>
#include <setjmp.h>

void fail() {
	int *a = (int*) malloc(sizeof(int)*10);
	a[100000] = 0;
}

jmp_buf env;

void mem_dump(int sugnum, siginfo_t *info, void *context_ref) {
	longjmp(env, 1000);
}

#if defined(__x86_64__)
void reg_dump(ucontext_t *uc) {
	printf("register dump\n\n");

	printf("RAX : %p\n", (void*)uc->uc_mcontext.gregs[REG_RAX]);
	printf("RBX : %p\n", (void*)uc->uc_mcontext.gregs[REG_RBX]);
	printf("RCX : %p\n", (void*)uc->uc_mcontext.gregs[REG_RCX]);
	printf("RDX : %p\n", (void*)uc->uc_mcontext.gregs[REG_RDX]);
	printf("RDI : %p\n", (void*)uc->uc_mcontext.gregs[REG_RDI]);
	printf("RSI : %p\n", (void*)uc->uc_mcontext.gregs[REG_RSI]);
	printf("RSP : %p\n", (void*)uc->uc_mcontext.gregs[REG_RSP]);
	printf("RBP : %p\n\n", (void*)uc->uc_mcontext.gregs[REG_RBP]);
}
#endif

void handle(int signum, siginfo_t *info, void *context_ref) {
	ucontext_t *uc = (ucontext_t*) context_ref;

#if defined(__x86_64__)
	reg_dump(uc);
#elif
	printf("Unsupported system\n");
#endif

	struct sigaction act;

	act.sa_flags = SA_NODEFER;
	act.sa_sigaction = mem_dump;

	if (sigaction(SIGSEGV, &act, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	char *mem_start = (char*)info->si_addr - 10;
	char *mem_end = (char*)info->si_addr + 10;

	printf("memory_dump\n\n");
	for (; mem_start != mem_end; ++mem_start) {
		printf("address 0x%zx : ", (size_t)mem_start);

		if (setjmp(env)) {
			printf("error\n");
		}
		else {
			printf("%02x\n", *mem_start);
		}
	}

	exit(2);
}

int main() {
	struct sigaction act;

	act.sa_flags = SA_NODEFER;
	act.sa_sigaction = handle;

	if (sigaction(SIGSEGV, &act, NULL) == -1) {
		perror("sigaction");
		return 1;
	}

	fail();

	return 0;
}
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

#define DEBUG 0

// ============================================================================

typedef struct sigaction signal_action;

static int pageSize;
static signal_action old_action;
char *p;
int how[3] = { PROT_NONE, PROT_READ, PROT_WRITE };

// ============================================================================

static void segv_handler(int signum, siginfo_t* info, void *context)
{
	int pageno, prot, rc;

	if (info->si_signo != SIGSEGV)
	{
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	pageno = ((char*)info->si_addr - p) / pageSize;

	if (how[pageno] == PROT_NONE)
	{
		prot = PROT_READ;
		how[pageno] = PROT_READ;

		fprintf(stderr, "Page %d now has read permissions. \n", pageno);
	}
	else
	{
		prot = PROT_READ | PROT_WRITE;
		fprintf(stderr, "Page %d now has read-write permissions\n", pageno);
	}

	rc = mprotect(p + pageno * pageSize, pageSize, prot);
	DIE(rc < 0, "mprotect");
}

// ============================================================================

static void set_signal(void)
{
	signal_action action;
	int rc;

	action.sa_sigaction = segv_handler;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, &old_action);
	DIE(rc == -1, "sigaction");
}

// ============================================================================

static void restore_signal(void)
{
	signal_action action;
	int rc;

	action.sa_sigaction = old_action.sa_sigaction;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, NULL);
	DIE(rc == -1, "sigaction");
}

// ============================================================================

int main(void)
{
	int i, rc;
	char ch;

	pageSize = getpagesize();

	p = mmap(NULL, 3 * pageSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	DIE(p == MAP_FAILED, "mmap(p)");

	for (i = 0; i != sizeof(how) / sizeof(*how); ++i)
	{
		rc = mprotect(p + i * pageSize, pageSize, how[i]);
		DIE(rc < 0, "mprotect");
	}

	set_signal();

	restore_signal();

	rc = munmap(p, 3 * pageSize);
	DIE(rc < 0, "munmap");

	return 0;
}

// ============================================================================
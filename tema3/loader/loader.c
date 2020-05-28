#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "exec_parser.h"

// ============================================================================

typedef struct sigaction signal_action;

// ============================================================================

static int page_size, descriptor;
static signal_action default_handler;
static so_exec_t* exec;

// ============================================================================

#define DIE(assertion, call_description)  \
	do 									  \
    {									  \
		if ((assertion))                  \
        {				                  \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(call_description);     \
			exit(123);				 	  \
		}						          \
	}                                     \
    while (0)

// ============================================================================

static inline int min(int x, int y)
{
	return (x < y) ? x : y;
}

// ============================================================================

static int get_segment(void* address)
{
	uintptr_t aux = (uintptr_t)address;
	for (int i = 0; i < exec->segments_no; ++i)
		if (aux >= exec->segments[i].vaddr)
			if (aux < exec->segments[i].vaddr + exec->segments[i].mem_size)
				return i;
	return -1;
}

// ============================================================================

static void sigsegv_handler(int signum, siginfo_t* info, void* context)
{
	void* mmap_pointer = NULL;
	void* address = NULL;
	int retval;
	char buffer[page_size];

	// ------------------------------------------------------------------------

	int segment_index = get_segment(info->si_addr);

	if (info->si_signo != SIGSEGV || segment_index < 0)
	{
		default_handler.sa_sigaction(signum, info, context);
		return;
	}

	so_seg_t* current_segment = &exec->segments[segment_index];
	int current_page = ((uintptr_t)info->si_addr - current_segment->vaddr)
					 / page_size;
	int offset = current_page * page_size;

	if (current_segment->data == NULL)
	{
		int size = current_segment->mem_size / page_size + 1;
		current_segment->data = calloc(size, sizeof(uint8_t));
	}

	if (((uint8_t*)current_segment->data)[current_page])
	{
		default_handler.sa_sigaction(signum, info, context);
		return;
	}
	else ++((uint8_t*)current_segment->data)[current_page];

	// ------------------------------------------------------------------------

	address = (void*)current_segment->vaddr + offset;
	mmap_pointer = mmap(address, page_size, PROT_READ | PROT_WRITE,
						MAP_ANON | MAP_SHARED | MAP_FIXED, -1, 0);
	DIE(mmap_pointer == MAP_FAILED, "mmap");

	int size = min(current_segment->file_size - offset, page_size);
	if (size > 0)
	{
		retval = lseek(descriptor, current_segment->offset + offset, SEEK_SET);
		DIE(retval < 0, "lseek");
		DIE(read(descriptor, buffer, size) < 0, "read");
		DIE(memcpy(mmap_pointer, buffer, size) == NULL, "memcpy");
	}

	retval = mprotect(mmap_pointer, page_size, current_segment->perm);
	DIE(retval < 0, "mprotect");
}

// ============================================================================

static void set_signal(void)
{
	signal_action handler;

	handler.sa_sigaction = sigsegv_handler;
	sigemptyset(&handler.sa_mask);
	sigaddset(&handler.sa_mask, SIGSEGV);
	handler.sa_flags = SA_SIGINFO;

	int retval = sigaction(SIGSEGV, &handler, &default_handler);
	DIE(retval == -1, "sigaction");
}

// ============================================================================

int so_init_loader(void)
{
	page_size = getpagesize();
	set_signal();
	return 0;
}

// ============================================================================

int so_execute(char* path, char* argv[])
{
	exec = so_parse_exec(path);
	if (!exec) return -1;

	descriptor = open(path, O_RDWR);
	DIE(descriptor < 0, "open");

	so_start_exec(exec, argv);

	return 0;
}

// ============================================================================
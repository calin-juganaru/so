#pragma once

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef char* string;

typedef enum {false, true} bool;

/* ========================================================================= */

#define _base_case(x, y) if (equals((x), (y)))
#define _case(x, y) else if (equals((x), (y)))
#define _default else

/* ========================================================================= */
#include <stdio.h>
#define DIE(assertion, call_description) \
	do                                   \
    {								     \
		if ((assertion))                 \
        {				                 \
			fprintf(stderr, "(%s, %d): ",\
					__FILE__, __LINE__); \
			perror(call_description);    \
			exit(1);				     \
		}						         \
	}                                    \
    while (false)

/* ========================================================================= */

/* ------------------------------------------------------------------------- */

bool equals(string x, string y)
{
	return (strcmp(x, y) == 0);
}

/* ------------------------------------------------------------------------- */

bool file_exists(string file)
{
	int test = open(file, O_RDONLY, 0444);
	if (test >= 0)
	{
		close(test);
		return true;
	}
	return false;
}

/* export LD_LIBRARY_PATH=D_LIBRARY_PATH:. */
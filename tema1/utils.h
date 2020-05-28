#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef char* string;
typedef FILE* stream;

typedef enum {false, true} bool;

const int MAX_LINE_SIZE = 256;
const int HASH_MAP_SIZE = 13;

const char* tokens = " \n#\t[]{}<>=+-*/%!&|^,:;()\\";

/* ========================================================================= */

#define _base_case(x, y) if (compare((x), (y)))
#define _case(x, y) else if (compare((x), (y)))
#define _default else

/* ========================================================================= */

#define DIE(assertion, call_description) \
	do                                   \
    {								     \
		if ((assertion))                 \
        {				                 \
			fprintf(stderr, "(%s, %d): ",\
					__FILE__, __LINE__); \
			perror(call_description);    \
			exit(errno);				 \
		}						         \
	}                                    \
    while (false)

/* ========================================================================= */

void delete(void* pointer)
{
	free(pointer);
	pointer = NULL;
}

/* ------------------------------------------------------------------------- */

void* alloc(int size)
{
	void* aux = malloc(size);
	if (aux == NULL) exit(12);
	return aux;
}

/* ------------------------------------------------------------------------- */

void* reloc(void* pointer, int size)
{
	void* aux = alloc(size);
	return memcpy(aux, pointer, size);
}

/* ------------------------------------------------------------------------- */

bool is_null(string str)
{
	return (str == NULL || str[0] == '\0' || str[0] == '\n');
}

/* ------------------------------------------------------------------------- */

void null(string str)
{
	if (!is_null(str)) str[0] = '\0';
}

/* ------------------------------------------------------------------------- */

string strdupl(string original)
{
	string copy = alloc((1 + strlen(original)) * sizeof(char));
    string aux = strncpy(copy, original, 1 + strlen(original));
	return aux;
}

/* ------------------------------------------------------------------------- */

bool file_exists(string file)
{
	stream test = fopen(file, "r");
	if (test != NULL)
	{
		fclose(test);
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------- */

bool compare(string x, string y)
{
	return (strcmp(x, y) == 0);
}

/* ------------------------------------------------------------------------- */

bool str_to_bool(string str)
{
	if (str != NULL && str[0] != '0')
		return true;
	return false;
}

/* ========================================================================= */
#pragma once

#include "utils.h"

/* ========================================================================= */

typedef struct _line_t
{
    string value;
    struct _line_t* next;
} line_t;

typedef struct
{
    line_t* first;
    line_t* last;
} file_t;

typedef line_t* list;
typedef file_t* file;

/* ========================================================================= */

file_t* create_file()
{
    file_t* file = alloc(sizeof(file_t));
    file->first = file->last = NULL;
    return file;
}

/* ------------------------------------------------------------------------- */

void pushback_line(file_t* file, string line)
{
    if (file->first == NULL)
    {
        file->first = alloc(sizeof(line_t));
        file->first->value = alloc(strlen(line) + 1);
        strcpy(file->first->value, line);

        file->last = malloc(sizeof(line_t));
        file->first->next = file->last;
    }
    else
    {
        file->last->value = alloc(strlen(line) + 1);
        strcpy(file->last->value, line);

        file->last->next = alloc(sizeof(line_t));
        file->last = file->last->next;
    }
}

/* ------------------------------------------------------------------------- */

void pushfront_line(file_t* file, string line)
{
    list aux = NULL;
    if (file->first == NULL)
    {
        file->first = alloc(sizeof(line_t));
        file->first->value = alloc(strlen(line) + 1);
        strcpy(file->first->value, line);

        file->last = alloc(sizeof(line_t));
        file->first->next = file->last;
    }
    else
    {
        aux = alloc(sizeof(line_t));
        aux->value = alloc(strlen(line) + 1);
        strcpy(aux->value, line);
        aux->next = file->first;
        file->first = aux;
    }
}

/* ------------------------------------------------------------------------- */

void print_file(file_t* file, stream output)
{
    line_t* line;
    for (line = file->first; line != file->last; line = line->next)
        fwrite(line->value, sizeof(char), strlen(line->value), output);
}
/* ------------------------------------------------------------------------- */

int get_file_size(file_t* file)
{
    line_t* line;
    int count = 0;
    for (line = file->first; line != file->last; line = line->next, ++count);
    return count;
}

/* ------------------------------------------------------------------------- */

void delete_file(file_t* file)
{
    list line, aux;

    for (line = file->first; line != file->last;)
    {
        aux = line;
        line = line->next;
        free(aux->value);
        free(aux);
    }

    free(file->last);
    free(file);
}

/* ========================================================================= */
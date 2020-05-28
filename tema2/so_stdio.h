#pragma once

#if defined(__linux__)
    #define FUNC_DECL_PREFIX

#elif defined(_WIN32)
    #include <Windows.h>

    #ifdef DLL_EXPORTS
        #define FUNC_DECL_PREFIX __declspec(dllexport)
    #else
        #define FUNC_DECL_PREFIX __declspec(dllimport)
    #endif

#else
    #error "Unknown platform"
#endif

#include "utils.h"

/* ========================================================================= */

#define SO_BUFFER_SIZE 4096

#define SEEK_SET 0	/* Seek from beginning of file. */
#define SEEK_CUR 1	/* Seek from current position.  */
#define SEEK_END 2	/* Seek from end of file.       */
#define SO_EOF (-1)

typedef struct
{
    char buffer[SO_BUFFER_SIZE];
    int first, last;
} SO_BUFFER;

typedef struct
{
    int file_descriptor;
    string mode;
    SO_BUFFER input, output;
    bool error, eof;
} SO_FILE;

/* ========================================================================= */

FUNC_DECL_PREFIX SO_FILE* so_fopen(const string path, const string mode);
FUNC_DECL_PREFIX int      so_fclose(SO_FILE* stream);
FUNC_DECL_PREFIX int      so_fgetc(SO_FILE* stream);
FUNC_DECL_PREFIX int      so_fputc(int c, SO_FILE* stream);
FUNC_DECL_PREFIX size_t   so_fread(void* ptr, size_t size,
                                   size_t nmemb, SO_FILE* stream);
FUNC_DECL_PREFIX size_t   so_fwrite(const void* ptr, size_t size,
                                    size_t nmemb, SO_FILE* stream);
FUNC_DECL_PREFIX int      so_fseek(SO_FILE* stream, long offset, int whence);
FUNC_DECL_PREFIX long     so_ftell(SO_FILE* stream);
FUNC_DECL_PREFIX int      so_fflush(SO_FILE* stream);
FUNC_DECL_PREFIX int      so_feof(SO_FILE* stream);
FUNC_DECL_PREFIX int      so_ferror(SO_FILE* stream);

FUNC_DECL_PREFIX SO_FILE* so_popen(const string command, const string type);
FUNC_DECL_PREFIX int      so_pclose(SO_FILE* stream);

#if defined(__linux__)
    FUNC_DECL_PREFIX int    so_fileno(SO_FILE* stream);
#elif defined(_WIN32)
    FUNC_DECL_PREFIX HANDLE so_fileno(SO_FILE* stream);
#else
    #error "Unknown platform"
#endif

/* ========================================================================= */
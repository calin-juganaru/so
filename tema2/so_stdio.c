#include "so_stdio.h"
#include <stdio.h>

/* ========================================================================= */

void zero_buffer(SO_BUFFER* so_buffer)
{
    memset(so_buffer->buffer, 0, SO_BUFFER_SIZE);
    so_buffer->first = 0;
    so_buffer->last = 0;
}

/* ========================================================================= */

int so_read(SO_FILE* stream, string buffer, int count)
{
    int bytes_read = 0, bytes_read_now;
    static int x = 0;

    while (bytes_read < count)
    {
        if (so_feof(stream))
        {
            stream->eof = true;
            return bytes_read;
        }

        bytes_read_now = read(stream->file_descriptor,
                              buffer + bytes_read,
                              count - bytes_read); ++x;
       /* printf("%d  %d  %d\n", x, bytes_read_now, bytes_read);*/

        if (bytes_read_now == 0)
        {
            return bytes_read;
        }

        if (bytes_read_now < 0)
            return SO_EOF;

        bytes_read += bytes_read_now;
    }

    return bytes_read;
}

/* ========================================================================= */

SO_FILE* so_fopen(const string path, const string mode)
{
    int descriptor, flags;
    SO_FILE* file;

    _base_case(mode, "r")
    {
        if (!file_exists(path)) return NULL;
        flags = O_RDONLY;
    }
    _case(mode, "r+")
    {
        if (!file_exists(path)) return NULL;
        flags = O_RDWR;
    }
    _case(mode, "w")
    {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    _case(mode, "w+")
    {
        flags = O_RDWR | O_CREAT | O_TRUNC;
    }
    _case(mode, "a")
    {
        flags = O_WRONLY | O_APPEND | O_CREAT;
    }
    _case(mode, "a+")
    {
        flags = O_WRONLY | O_APPEND | O_CREAT;
    }
    _default
    {
        return NULL;
    }

    descriptor = open(path, flags, 0644);
    DIE(descriptor < 0, "open");

    file = malloc(sizeof(SO_FILE));
    memset(file, 0, sizeof(SO_FILE));

    file->input.first = file->input.last = 0;
    zero_buffer(&file->output);
    file->file_descriptor = descriptor;

    return file;
}

/* ========================================================================= */

int so_fclose(SO_FILE* stream)
{
    int aux = 0;

    if(stream != NULL)
    {
        aux = so_fflush(stream);
        aux = close(stream->file_descriptor);

        free(stream);
        stream = NULL;
    }

    if (aux)
    {
        stream->error = true;
        return SO_EOF;
    }

    return 0;
}

/* ========================================================================= */

int so_fgetc(SO_FILE* stream)
{
    int return_value;

    if (stream->input.first == stream->input.last)
    {
        if (stream->eof) return -2;

        zero_buffer(&stream->input);
        return_value = so_read(stream,
                               stream->input.buffer,
                               SO_BUFFER_SIZE);

        if (return_value < 0)
        {
            stream->error = true;
            return SO_EOF;
        }

        stream->input.last = return_value;
    }

    return stream->input.buffer[stream->input.first++];
}

/* ========================================================================= */

size_t so_fread(void* ptr, size_t size, size_t nmemb, SO_FILE* stream)
{
    int i, j;
    unsigned char aux;
    size_t count = 0;

    for (i = 0; i < nmemb; ++i)
        for (j = 0; j < size; ++j)
        {
            aux = so_fgetc(stream);
            if (aux >= 0)
            {
                ++count;
                *(char*)(ptr + i * size + j) = aux;
            }
            else return count / size;
        }

    return count / size;
}

/* ========================================================================= */

int so_write(int descriptor, const string buffer, int count)
{
    int bytes_written = 0, bytes_written_now;

    while (bytes_written < count)
    {
        bytes_written_now = write(descriptor, buffer + bytes_written,
                                  count - bytes_written);
        bytes_written += bytes_written_now;

        if (bytes_written_now < 0)
            return -1;

        bytes_written += bytes_written_now;
    }

    return bytes_written;
}

/* ========================================================================= */

int so_fputc(int c, SO_FILE* stream)
{
    int return_value, size = 0;

    if (stream->output.last == SO_BUFFER_SIZE)
    {
        return_value = so_write(stream->file_descriptor,
                                stream->output.buffer, SO_BUFFER_SIZE);
        if (!return_value)
            return 0;
        else if (return_value < 0)
        {
            stream->error = true;
            return SO_EOF;
        }

        zero_buffer(&stream->output);
    }

    stream->output.buffer[stream->output.last++] = c;

    return c;
}

/* ========================================================================= */

size_t so_fwrite(const void* ptr, size_t size, size_t nmemb, SO_FILE* stream)
{
    int i, j, return_value;
    unsigned char aux;
    size_t count = 0;

    for (i = 0; i < nmemb; ++i)
        for (j = 0; j < size; ++j)
        {
            aux = *(char*)(ptr + i * size + j);
            return_value = so_fputc(aux, stream);

            if (return_value != SO_EOF)
            {
                ++count;
            }
            else return count / size;
        }

    return count / size;
}

/* ========================================================================= */

int so_fflush(SO_FILE* stream)
{
    int count;

    if (stream->output.last == 0)
        return 0;

    count = so_write(stream->file_descriptor,
                     stream->output.buffer,
                     stream->output.last);

    if (count < 0)
    {
        stream->error = true;
        return SO_EOF;
    }

    zero_buffer(&stream->output);

    return 0;
}

/* ========================================================================= */

int so_fseek(SO_FILE* stream, long offset, int whence)
{
    int aux = lseek(stream->file_descriptor, offset, whence);
    if (aux < 0)
    {
        stream->error = true;
        return -1;
    }
    return 0;
}

/* ========================================================================= */

long so_ftell(SO_FILE* stream)
{
    return lseek(stream->file_descriptor, 0, SEEK_CUR);
}

/* ========================================================================= */

int so_fileno(SO_FILE* stream)
{
    return stream->file_descriptor;
}

/* ========================================================================= */

int so_feof(SO_FILE* stream)
{
    int current_position, end_of_file;

    current_position = so_ftell(stream);
    so_fseek(stream, 0, SEEK_END);
    end_of_file = so_ftell(stream);
    so_fseek(stream, current_position, SEEK_SET);

    return (current_position == end_of_file);
}

/* ========================================================================= */

int so_ferror(SO_FILE* stream)
{
    return stream->error;
}

/* ========================================================================= */

SO_FILE* so_popen(const string command, const string type)
{
    return NULL;
}

/* ========================================================================= */

int so_pclose(SO_FILE* stream)
{
    return 0;
}

/* ========================================================================= */
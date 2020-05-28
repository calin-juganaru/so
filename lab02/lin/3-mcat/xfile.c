#include <stdio.h>

#include "xfile.h"
#include "utils.h"

int xread(int fd, void* buf, int count)
{
    int bytes_read = 0, bytes_read_now;

    while (bytes_read < count)
    {
        bytes_read_now = read(fd, buf + bytes_read, count - bytes_read);
        bytes_read += bytes_read_now;

        DIE(bytes_read_now < 0, "I/O error");

        if (bytes_read_now == 0)
            return bytes_read;
    }

    return bytes_read;
}

int xwrite(int fd, const void* buf, int count)
{
    int bytes_written = 0, bytes_written_now;

    while (bytes_written < count)
    {
        bytes_written_now = write(fd, buf + bytes_written, count - bytes_written);
        bytes_written += bytes_written_now;

        DIE(bytes_written_now < 0, "I/O error");
    }

    return bytes_written;
}

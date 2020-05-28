#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "xfile.h"
#include "utils.h"

#define BUFFSIZE 32

int main(int argc, char* argv[])
{
	int fd_src, fd_dst, rc, bytes_read, bytes_written;
	char buffer[BUFFSIZE];

	if (argc < 2 || argc > 3)
	{
		printf("Usage:\n\t%s source_file [destination_file]\n", argv[0]);
		return 0;
	}

	fd_src = open(argv[1], O_RDONLY);
	DIE(fd_src < 0, "open error");

	if (argc == 3)
	{
		fd_dst = open(argv[2], O_WRONLY | O_CREAT, 0644);

		rc = close(STDOUT_FILENO);
		DIE(rc < 0, "close stdout");
		rc = dup(fd_dst);
		DIE(rc < 0, "dup fd1");
	}

	while (bytes_read = read(fd_src, buffer, BUFFSIZE))
	{
		DIE(bytes_read < 0, "read error");

		bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
		while (bytes_written < bytes_read)
		{
			bytes_written += write(STDOUT_FILENO, buffer + bytes_written, bytes_read - bytes_written);
		}
	}

	close(fd_src);
	close(fd_dst);
	close(rc);

	return 0;
}

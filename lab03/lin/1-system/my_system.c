#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

int main(int argc, char *argv[])
{
	int i, ret;
	char* str = malloc(100);

	if (argc < 2)
	{
		printf("Usage: ./system cmd\n");
		free(str);
		return 0;
	}

	if (argc < 3)
	{
		ret = system(argv[1]);
		DIE(ret < 0, "system");
	}
	else
	{
		for (i = 1; i < argc; ++i)
		{
			strcat(str, argv[i]);
			strcat(str, " ");
		}
		ret = system(str);
		DIE(ret < 0, "system 2");
	}

	free(str);
	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#define MAX_LINE_SIZE	256
#define MAX_ARGS		8

#define ERROR			0
#define SIMPLE			1
#define REDIRECT		2
#define PIPE			3
#define SET_VAR			4
#define SHELL_EXIT		5

static char* verb;
static char** args;
static char* stdin_file;
static char* stdout_file;
static char* stderr_file;

static char* value;
static char* var;

static int type;

static int parse_line(char *line);
static void alloc_mem(void);
static void free_mem(void);

static void do_redirect(int filedes, const char* filename)
{
	int rc, fd;

	fd = open(filename, O_TRUNC | O_CREAT | O_RDWR, 0644);
	DIE(fd < 0, "fopen");
	rc = dup2(fd, filedes);
	DIE(rc < 0, "dup2");
}

static void set_var(const char* name, const char* value)
{
	setenv(name, value, 1);
}

static char* expand(const char* name)
{
	return getenv(name);
}

static void simple_cmd(char* args[])
{
	pid_t pid, wait_ret;
	int status;
	char* cmd = malloc(50);

	DIE(cmd == NULL, "cmd malloc");

	pid = fork();

	switch (pid)
	{
		case -1:
			DIE(pid, "PID");
			break;

		case 0:
			if (stdout_file != NULL)
				do_redirect(STDOUT_FILENO, stdout_file);

			strcat(cmd, "/bin/");
			strcat(cmd, args[0]);
			execv(cmd, args + 1);
			break;

		default:
			wait_ret = waitpid(pid, &status, 0);
			DIE(wait_ret < 0, "wait ret");
			break;
	}

	free(cmd);
}

int main(void)
{
	char line[MAX_LINE_SIZE];

	alloc_mem();

	while (1)
	{
		printf("> ");
		fflush(stdout);

		memset(line, 0, MAX_LINE_SIZE);

		if (fgets(line, sizeof(line), stdin) == NULL) {
			free_mem();
			exit(EXIT_SUCCESS);
		}

		type = parse_line(line);

		switch (type) {
		case SHELL_EXIT:
			free_mem();
			exit(EXIT_SUCCESS);

		case SET_VAR:
			set_var(var, value);
			break;

		case SIMPLE:
			simple_cmd(args);
			break;
		}
	}

	return 0;
}

static void alloc_mem(void)
{
	args = malloc(MAX_ARGS * sizeof(char *));
	DIE(args == NULL, "malloc");
}

static void free_mem(void)
{
	free(args);
}

static int parse_line(char *line)
{
	int ret = SIMPLE, idx = 0;
	char* token = NULL;
	char* delim = "=\n";

	stdin_file = NULL;
	stdout_file = NULL;
	stderr_file = NULL;

	/* check for exit */
	if (!strncmp("exit", line, strlen("exit")))
		return SHELL_EXIT;

	/* var = value */
	if (strchr(line, '=') != 0)
	{
		token = strtok(line, delim);

		if (token == NULL)
			return ERROR;

		var = strdup(token);

		token = strtok(NULL, delim);
		if (token == NULL)
			return ERROR;

		value = strdup(token);
		return SET_VAR;
	}

	/* normal command */
	delim = " \t\n";
	token = strtok(line, delim);

	if (token == NULL)
		return ERROR;

	verb = strdup(token);

	while (token != NULL)
	{
		if (token[0] == '$')
			token = expand(token + 1);

		if (token == NULL)
		{
			printf(" Expansion failed\n");
			return ERROR;
		}

		if (!strncmp(token, ">", strlen(">")))
		{
			if (strlen(token) > strlen(">"))
			{
				token++;
				stdout_file = strdup(token);
			}
			else
			{
				token = strtok(NULL, delim);
				stdout_file = strdup(token);
			}
		}
		else
		{
			args[idx++] = strdup(token);
		}

		token = strtok(NULL, delim);
	}

	args[idx++] = NULL;
	return ret;
}

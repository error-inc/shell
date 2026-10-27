#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_ARGS 64

typedef enum {
	PLAIN, IN_SINGLE, IN_DOUBLE
} char_state;

static int run_builtin (char** argv)
{
	if (strcmp(argv[0],"cd") == 0)
	{
		if (argv[1] != NULL && argv[2] != NULL)
		{
			fprintf(stderr,"cd: too many arguments\n");
			return 1;
		}
		char* target = argv[1] ? argv[1] : getenv("HOME");
		if(target == NULL) 
			fprintf(stderr,"cd: HOME not set\n");
		else if (chdir(target) != 0) 
			perror("cd");
		return 1;
	}
	return 0;
}

static int tokenizer(char* line, char** argv, int max)
{
	char_state state = PLAIN;
	int r=0;
	int w = 0;
	int count = 0;
	while(line[r] == ' ' || line[r] == '\t') r++;
	while(line[r] != '\0')
	{
		argv[count] = &line[w];
		while(line[r] != '\0')
		{
			if(state == IN_SINGLE)
			{
				if(line[r] == '\'') state = PLAIN;	
				else line[w++] = line[r];
			}
			else if(state == IN_DOUBLE)
			{
				if(line[r] == '"') state = PLAIN;
				else line[w++] = line[r];
			}
			else
			{
				if(line[r] == ' ' || line[r] == '\t')
				{
					break;
				}
				if(line[r] == '\'')
					state = IN_SINGLE;
				else if(line[r] == '"')
					state = IN_DOUBLE;
				else
					line[w++] = line[r];
			}
			r++;
		}
		int at_end = (line[r] == '\0');
		line[w++] = '\0';
		if (!at_end) r++;
		if (count == max-1) return -1;
		count++;
		while(line[r] == ' ' || line[r] == '\t') r++;
	}
	if(state != 0) 
		return -2;
	argv[count] = NULL;

	return 0;
}	

static int run_external(char** argv)
{	
	pid_t p = fork();
	if (p < 0)
	{
		perror("Fork Failed!");
		return 1;
	}
	if (p == 0)
	{
		execvp(argv[0], argv);
		perror(argv[0]);
		_exit(127);
	}

	int status;
	waitpid(p, &status, 0);
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);
	return 1;
}

static int read_line(char *buf, size_t size)
{
	if (fgets(buf, (int)size, stdin) == NULL)
		return 0;

	if (strchr(buf, '\n') == NULL && !feof(stdin))
	{
		int ch;
		while ((ch = getchar()) != '\n' && ch != EOF);
		return -1;
	}

	buf[strcspn(buf, "\n")] = '\0';
	return 1;
}
				
int main(void)
{
	char c[1024];
	[[maybe_unused]] int last_status = 0;
	while (1)
	{
		printf(">");
		fflush(stdout);

		int got = read_line(c, sizeof(c));
		if (got == 0)
			break;
		if (got == -1)
		{
			fprintf(stderr, "Line too long (limit %zu characters)\n", sizeof(c) - 1);
			continue;
		}

		char *argv[MAX_ARGS];
		int token_result = tokenizer(c, argv, MAX_ARGS);
		if (token_result == -2)
		{
			fprintf(stderr, "Syntax Error: unclosed quote\n");
			continue;
		}
		if (token_result == -1)
		{
			fprintf(stderr, "Too many arguments (limit %d)\n", MAX_ARGS - 1);
			continue;
		}
		if (argv[0] == NULL)
			continue;

		if (strcmp(argv[0], "exit") == 0)
			break;
		if (run_builtin(argv))
			continue;
		last_status = run_external(argv);
	}
	if (feof(stdin))
		printf("\n");
	return 0;
}

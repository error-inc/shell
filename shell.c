#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int run_builtin (char** argv)
{
	if (strcmp(argv[0],"cd") == 0)
	{
		if (argv[2] != NULL)
		{
			fprintf(stderr,"cd: too many arguments\n");
			return 1;
		}
		char* target = argv[1] ? argv[1] : getenv("HOME");
		if(target == NULL) 
			fprintf(stderr,"cd: Home not set\n");
		else if (chdir(target) != 0) 
			perror("cd");
		return 1;
	}
	return 0;
}

int main(void)
{
	char c[1024];
	while(1)
	{
		printf(">");
		fflush(stdout);
		if(fgets(c, sizeof(c), stdin) == NULL)
		{
			break;
		}
		c[strcspn(c, "\n")] = '\0';
		char* argv[64];
		argv[0] = strtok(c, " \t");
		if (argv[0] == NULL)
		{
			continue;
		}
		int i = 1;
		argv[63] = NULL;
		while(i < 63)  // Null terminated array argv[63] should always be null
		{
			argv[i] = strtok(NULL, " \t");
			if (argv[i] == NULL)
			{
				break;
			}
			i++;
		}
		if (strcmp(argv[0],"exit") == 0)
		{
			break;
		}
		if (run_builtin(argv))
		{
			continue;
		}
		pid_t p = fork();
		if (p < 0)
		{
			perror("Fork Failed!");
			continue;
		}
		if (p == 0)
		{
			execvp(argv[0], argv);
			perror(argv[0]);
			_exit(1);
		}
		else
		{
			int status;
			waitpid(p, &status, 0);
		}

	}
	if (feof(stdin))
	{
		printf("\n");
	}
	return 0;
}

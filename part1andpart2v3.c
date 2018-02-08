#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXCHAR 512

void readInput();
void executeCommand(char *command, char **argv);
int getNumberOfTokens(char **tokens);

int main()
{
	char **input;
	char path[100];

	// Print PATH environment variable
	printf("PATH IS -> %s\n", getenv("PATH"));
	readInput();
	return 0;
}

/*
Reads,parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
void readInput()
{
	int token_cnt = 0;
	int i = 0;
	int  args_len;
	char input[MAXCHAR] = "";
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	char *command;
	char **tokens;
	const char exit_command[6] = "exit\n";
	char c, systemsymbol = '$';

	tokens = (char **)malloc(50 * sizeof(char *));
	//argv = (char **)malloc(50 * sizeof(char *));
	for (int i = 0; i < 50; i++)
	{
		tokens[i] = (char *)malloc(MAXCHAR * sizeof(char));
	}

	printf("Tokens array successfully created!\n");

	while (1)
	{
		printf("%c ", systemsymbol);

		// Initialize argv -> memory allocation
		for (int i = 0; i < 50; i++)
		{
			tokens[i] = (char *)malloc(MAXCHAR * sizeof(char));
		}

		fgets(input, MAXCHAR, stdin);
		//Checks if "exit" or Ctrl + D are present in the input...
		if ((strcmp(exit_command, input) == 0) || feof(stdin))
		{
			printf("\n");
			break;
		}

		//Checks if the input is more than 512 symbols...
		if (input[strlen(input) - 1] != '\n')
		{
			printf("The command is too long!\n");
			printf("%c ", systemsymbol);
			//The following line flushes the buffer after the 512th symbol...
			while ((c = getchar()) != '\n' && c != EOF)
			{
			}
			strcpy(input, "");
		}

		token = strtok(input, delimiters);
		//if(token != NULL)
		{
		//	command = strdup(token);
		}
		args_len = 0;
		while (token != NULL)
		{
			if (strcmp(token, "\n") != 0)
			{
				strcpy(tokens[token_cnt], token);
				//strcpy(argv[args_len], token);
				args_len++;
				printf("COPYING '%s' into tokens[%d]\n", token, token_cnt);
				token_cnt++;
			}
			token = strtok(NULL, delimiters);
		}
		tokens[args_len] = NULL;
		executeCommand(tokens[0], tokens);

		// Free argv -> reset it
		for (int i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
	}

	//Tests if the tokens have been stored correctly...
	printf("\n");
	for(i = 0; i < token_cnt; i++) {
		printf("TOKEN[%d]: %s\n", i, tokens[i]);
	}
}

void executeCommand(char *command, char **args)
{
	pid_t pid;
	int pid_status;
	int exec_status;

	pid = fork();

	// Fork a child process
	if (pid < 0)
	{
		// Fork error
		printf("Fork Failed\n");
		exit(1);
	}
	else if (pid == 0)
	{
		// Child process
		exec_status = execvp(command, args);
		if (exec_status < 0)
		{
			//printf("Command Exec Failed: is your command valid?\n");
			perror(command);
			exit(2);
		}
	}
	else
	{ // Wait for completition
		while (wait(&pid_status) != pid)
			;
	}
}

int getNumberOfTokens(char **tokens)
{
	int i = 0, cnt = 0;

	while (strlen(tokens[i]) > 0)
	{
		i++;
		cnt++;
	}
	return cnt;
}

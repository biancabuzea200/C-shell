#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXCHAR 512

int readInput();
void executeCommand(char *command, char **argv);
int getNumberOfTokens(char **tokens);

int main()
{
	printf("PATH IS -> %s\n", getenv("PATH"));
	while(1)
	{
		if(readInput() < 0)
		{
			break;		
		}
	}
		
	return 0;
}

/*
Reads,parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
int readInput()
{
	char input[MAXCHAR];
	const char delimiters[13] = " \t<>|;&\n";
	char **tokens;
	const char exit_command[6] = "exit\n";
	char systemsymbol = '$';
	char *token;
	
	token = (char *)malloc(50 * sizeof(char *));
	tokens = (char **)malloc(50 * sizeof(char *));
	for (int i = 0; i < 50; i++)
	{
		tokens[i] = (char *)malloc(50 * sizeof(char));
	}

	printf("%c", systemsymbol);
	
	fgets(input, MAXCHAR, stdin);
		
	//Checks if exit or Ctrl + D are present in the input
	
	if ((strcmp(exit_command, input) == 0) || feof(stdin))
	{
		printf("\n");
		return -1;
	}

	//Checks if the input is more than 512 symbols...
	if (input[strlen(input) - 1] != '\n')
	{
		printf("The command is too long!\n");
		strcpy(input, "");
	}

	token = strtok(input, delimiters);
	int tcount = 0;
	while(token != NULL)
	{
		if (strcmp(token, "\n") != 0)
		{
			strcpy(tokens[tcount], token);
			tcount++;
		}
		token = strtok(NULL, delimiters);
	}
		
	executeCommand(tokens[0], tokens);

	// Free tokens -> reset it
	for (int i = 0; tokens[i] != NULL; i++)
	{
		free(tokens[i]);
	}
	return 0;
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
			perror(command);
			exit(2);
		}
	}
	else
	{
		// Wait for completition
		while (wait(&pid_status) != pid);
	}
}

int getNumberOfTokens(char **tokens)
{
	int cnt = 0;

	while (strlen(tokens[cnt]) > 0)
	{
		cnt++;
	}
	return cnt;
}

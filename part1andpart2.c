#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXCHAR 512

char **readInput(char **tokens);
void executeCommand(char **tokens);
int getNumberOfTokens(char **tokens);

int main()
{
	char **input;
	char path[100];

	// Print PATH environment variable
	printf("PATH IS -> %s\n", getenv("PATH"));
	
	input = readInput(input);

	return 0;
}

/*
Reads,parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
char **readInput(char **tokens)
{
	int token_cnt = 0; 
	char input[MAXCHAR] = "";
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	const char exit_command[6] = "exit\n";
	char c, systemsymbol = '$';

	tokens = (char **)malloc(50 * sizeof(char *));

	
	printf("Tokens array successfully created!\n");

	while (1)
	{
		
		token_cnt = 0;		
		
		//for (int i = 0; i < 50; i++)
		//{
		//tokens[i] = (char *)malloc(MAXCHAR * sizeof(char));
		//}
		
		printf("%c ", systemsymbol);

		
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
		
		while (token != NULL)
		{
			if (strcmp(token, "\n") != 0)
			{
				
				tokens[token_cnt]=strdup(token);
				printf("COPYING '%s' into tokens[%d]\n", token, token_cnt);
				token_cnt++;
			}
			token = strtok(NULL, delimiters);
		}
		tokens[token_cnt] = NULL;
		
		if(tokens[0] != NULL)
		{		
			executeCommand(tokens);
		}
		// Free tokens -> reset it
		for (int i = 0; i < token_cnt; i++)
		{
			free(tokens[i]);
		}
	}
	return tokens;
}

void executeCommand(char **tokens)
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
		exec_status = execvp(tokens[0], tokens);
		if (exec_status < 0)
		{
			perror(tokens[0]);
			exit(2);
		}
	}
	else
	{ // Wait for completition
		while (wait(&pid_status) != pid)
			;
	}
}

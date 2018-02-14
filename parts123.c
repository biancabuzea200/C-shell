#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAXCHAR 512

char **readInput(char **tokens);
void setToHome();
void setPath(char *token);
void printPath();
void execute_command(char **tokens, char *initPath);
int getNumberOfTokens(char **tokens);

int main()
{
	char **input;

	// Print PATH environment variable
	printf("PATH IS -> ");
	printPath();

	setToHome();
	input = readInput(input);

	return 0;
}

/*
Reads, parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
char **readInput(char **tokens)
{
	int token_cnt = 0;
	char input[MAXCHAR] = "";
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	const char exit_command[6] = "exit\n";
	char c, *systemsymbol = "$ ";
	char *path;

	tokens = (char **)malloc(50 * sizeof(char *));
	printf("Tokens array successfully created!\n");

	path = strdup(getenv("PATH"));
	// Print PATH environment variable
	printf("PATH IS -> %s\n", path);

	while (1)
	{
		token_cnt = 0;
		printf("%s", systemsymbol);

		fgets(input, MAXCHAR, stdin);

		//Checks if "exit" or Ctrl + D are present in the input...
		if ((strcmp(exit_command, input) == 0) || feof(stdin))
		{
			if (feof(stdin))
				printf("\n");
			exit(0);
		}

		// Checks if the input is more than 512 symbols...
		if (input[strlen(input) - 1] != '\n')
		{
			printf("The command is too long!\n");

			// The following line flushes the buffer after the 512th symbol...
			while ((c = getchar()) != '\n' && c != EOF)
			{
			}

			// Clears the input
			strcpy(input, "");

			// Get the next command
			continue;
		}

		token = strtok(input, delimiters);

		while (token != NULL)
		{
			if (strcmp(token, "\n") != 0)
			{
				tokens[token_cnt] = strdup(token);
				printf("COPYING '%s' into tokens[%d]\n", token, token_cnt);
				token_cnt++;
			}
			token = strtok(NULL, delimiters);
		}
		tokens[token_cnt] = NULL;

		if (tokens[0] != NULL)
		{
			execute_command(tokens, path);
		}
	}
	return tokens;
}

/*
The following function sets the Environment variable HOME
Parameters: none
*/
void setToHome()
{
	char current_dir[100];
	if (chdir(getenv("HOME")) == 0)
	{
		printf("CURRENT DIRECTORY SUCCESSFULLY CHANGED !\n");
		getcwd(current_dir, 100);
		printf("CURRENT DIRECTORY IS -> %s\n", current_dir);
	}
	else
		printf("CURRENT DIRECTORY HASN'T BEEN CHANGED!");
}

/*
The following function prints the Environment variable PATH
Parameters: none
*/
void printPath()
{
	printf("%s\n", getenv("PATH"));
}

/*
The following function sets the Environment variable PATH
Parameters: char* token
*/
void setPath(char *token)
{
	struct stat buffer;
	int status;

	status = lstat(token, &buffer);

	if (status != 0)
	{
		printf("Invalid path!\n");
	}
	else if (setenv("PATH", token, 1) != 0)
	{
		printf("Error while setting the path!\n");
	}
}

/*
The following function executes the commands. 
Parameters: char** tokens and char* initPath.
*/
void execute_command(char **tokens, char *initPath)
{
	pid_t pid;
	int pid_status;
	int exec_status;

	if (strcmp(tokens[0], "getpath") == 0)
	{
		if (getNumberOfTokens(tokens) == 1)
		{
			printPath();
		}
		else
		{
			printf("Error! Invalid number of arguments for 'getpath'\n");
		}
	}
	else if (strcmp(tokens[0], "setpath") == 0)
	{
		if (getNumberOfTokens(tokens) == 2)
		{
			setPath(tokens[1]);
		}
		else
		{
			printf("Error! Invalid number of arguments for 'setpath'\n");
		}
	}
	else
	{
		pid = fork();

		// Fork a child process
		if (pid < 0)
		{
			// Fork error
			printf("Fork Failed\n");
			setPath(initPath);

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
		{
			// Wait for completition
			while (wait(&pid_status) != pid)
				;
		}
	}
}

/*
The following function returns the number of tokens entered per line.
Parameters: char** tokens
*/
int getNumberOfTokens(char **tokens)
{
	int i = 0;

	while (tokens[i] != NULL)
	{
		i++;
	}
	return i;
}

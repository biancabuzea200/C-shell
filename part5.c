#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

#define MAXCHAR 512
#define HISTORY_COUNT 20


typedef struct history_command
{

int counter;
char* command;

}history_command;



void readInput(char **tokens);
void setToHome();
void setPath(char *token);
void printPath();
int execute_command(char **tokens);
int getNumberOfTokens(char **tokens);
void wrongNumOfTokensError(char *command);
int numberOfCommands();
//int clear_history( char *history[]);
void display_history();
char** parseHistory(char* history);
int isNumber(char* string);
int lastCommand();

history_command history[20];


int main()
{
	char **input;
	char *path;


	// Print PATH environment variable
	printf("PATH IS -> ");
	printPath();

	// Save the PATH to restore it later
	path = strdup(getenv("PATH")); 

	// Set the current direcotry to home
	setToHome();

	// Read the input from the user
	readInput(input);

	// Restore the original PATH
	setPath(path);
	printf("PATH IS -> ");
	printPath();
	printf("INDEX OF LAST COMMAND IS: %d\n",lastCommand());
	return 0;
}

/*
Reads, parses the user input and returns the program return status
Parameters: char tokens[MAXCHAR]
*/
void readInput(char **tokens)
{
	int token_cnt = 0; // number of tokens
	char input[MAXCHAR] = "";
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	history_command command1;//structure instance
	char c, *systemsymbol = "$ ";
	int command_cnt = 0; 
	int index = 0; //index in the array
	
	tokens = (char **)malloc(50 * sizeof(char *));
	printf("Tokens array successfully created!\n");

	while (1)
	{	
		
		token_cnt = 0;
		
		printf("%s", systemsymbol);

		fgets(input, MAXCHAR, stdin);
		
	
		 if (strcspn(input,"!") != 0 )
		{
			command_cnt++;	
			
		}
		
		
		command1.command = strdup(input);
		command1.counter = command_cnt;

		if (feof(stdin))
		{
			printf("\n");
			return;
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
				
				token_cnt++;
			}
			token = strtok(NULL, delimiters);
		}
		tokens[token_cnt] = NULL;
			
		if (strcspn(tokens[0],"!") != 0)
		{	
							
			history[(command_cnt-1)%20] = command1; 
		}	
		if (tokens[0] != NULL)
		{						
			if(execute_command(tokens) == -1)			
			break;			
		}	
  }
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
		if (setenv("PATH",token,1) != 0)
		{
		printf("Error while setting the path!\n");
		}	
}

/*
The following function executes the commands. The function returns -1 if the exit command is invoked.
Parameters: char** tokens and char* initPath.
*/
int execute_command(char **tokens)
{
	pid_t pid;
	int pid_status;
	const char exit_command[6] = "exit";
	char commandNumber[2];

	if (strcmp(tokens[0], "getpath") == 0)
	{
		if (getNumberOfTokens(tokens) == 1)
		{
			printPath();
		}
		else
		{
			wrongNumOfTokensError("getpath");
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
			wrongNumOfTokensError("setpath");
		}
	}
	else if (strcmp(tokens[0], "cd") == 0)
	{
		if (getNumberOfTokens(tokens) == 1)
		{
			setToHome();
		}
		else if (getNumberOfTokens(tokens) == 2)
		{
			if (chdir(tokens[1]) != 0)
			{
				perror(tokens[1]);
			}
		}
		else
		{
			wrongNumOfTokensError("cd");
		}
	}

	else if(strcmp(exit_command, tokens[0]) == 0)
	{

	 	if(getNumberOfTokens(tokens) > 1){
		printf("Invalid number of parameters for exit command!\n");
		return 0;
		}
		else
		{
		 return -1;
		}		
					
	}
	// this works
	else if(strcmp("history", tokens[0]) == 0)
	{
		if(numberOfCommands() == 0)
		printf("There are no commands stored!\n");
		
		else
		display_history();				
	}
	else if(strcspn(tokens[0],"!") == 0 && tokens[0][1] != '!' && tokens[0][1] != '-')
	{			
		if (strlen(tokens[0]) < 3)		
		{ 
		memmove(commandNumber,tokens[0]+1,1); 		
		}
		else
		{ 
		memmove(commandNumber,tokens[0]+1,2); 
		}
		
		if(getNumberOfTokens(tokens) != 1)
		{ 
			printf("Invalid number of arguments!\n");
		}	
		else if(atoi(commandNumber) > numberOfCommands())
		{
		printf("Invalid number of command!\n");		
		}
		else if(isNumber(commandNumber) == 1)
		{
		printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands());
		}		
		
		else
		{
			if (strcmp(history[atoi(commandNumber)-1].command, "\n") != 0)		
			execute_command(parseHistory(history[atoi(commandNumber)-1].command));			
		}
	
	}
		//Executes last command in history.
	else if(strcspn(tokens[0],"!") == 0 && tokens[0][1] == '!')
	{	
		execute_command(parseHistory(history[lastCommand()].command));
		
	}
		//Executes Last command - a number
	else if (strcspn(tokens[0],"!") == 0 && tokens[0][1] == '-')
		{
			if (strlen(tokens[0]) < 4)		
			{ 	
				memmove(commandNumber,tokens[0]+2,1);
				 		
			}
			else
			{ 	
				memmove(commandNumber,tokens[0]+2,2);
				  
			}
		
			if(getNumberOfTokens(tokens) != 1)
			{ 
				printf("Invalid number of arguments!\n");
			}	
			else if(atoi(commandNumber) >= numberOfCommands())
				{
					printf("Invalid number of command!\n");		
				}
			else if(isNumber(commandNumber) == 1)
				{
					printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands());
				}		
		
			else
			{
				if (strcmp(history[atoi(commandNumber)-1].command, "\n") != 0)		
				{	 			
					if (lastCommand() - atoi(commandNumber) >= 0)					
						execute_command(parseHistory(history[lastCommand() - atoi(commandNumber)].command));
					else	
						execute_command(parseHistory(history[20 - (atoi(commandNumber) - lastCommand())].command));								
				}			
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
			exit(1);
		}
		else if (pid == 0)
		{
			// Child process
			execvp(tokens[0], tokens);
			
				perror(tokens[0]);
				exit(2);
			
		}
		else
		{
			// Wait for completition
			while (wait(&pid_status) != pid);
		}
	
	}
	return 0;
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

/*
The folowing function prints an error if the number of arguments for a command is wrong
Parameters: char* command
*/
void wrongNumOfTokensError(char *command)
{
	printf("Error! Invalid number of arguments for '%s'\n", command);
} 
/*
The following function displays the history , prints history number which starts from one.
*/
void display_history () 
{
int history_number = 1;
while(history[history_number-1].counter)
{
	printf("%4d %s\n" , history_number , history[history_number-1].command);
	history_number++;	
	}	
}

//this works 
int numberOfCommands()
{
int i = 0;
while(history[i].counter != 0)
{
 i++;
}
return i;
}
/*
This parses History inputs.
*/
char** parseHistory(char* history)
{
	int token_cnt = 0; // number of tokens
	const char delimiters[13] = " <>\n";
	char *token = "";
	char **parsedHistory;
	parsedHistory = (char **)malloc(20 * sizeof(char *));

		token = strtok(history, delimiters);

		while (token != NULL)
		{

			if (strcmp(token, "\n") != 0)
			{
				parsedHistory[token_cnt] = strdup(token);
				
				token_cnt++;
			}
			token = strtok(NULL, delimiters);
		}
		parsedHistory[token_cnt] = NULL;

return parsedHistory;
}
/*
Checks if a given string is a number.
*/
int isNumber(char* string){

int length = strlen(string);

for (int j = 0; j<length-1; j++){

	if (!isdigit(string[j]))
	return 1;
}
return 0;
}
/*
Returns the index of the last command in history array.
*/
int lastCommand()
{
int index = 0;
int max = 0;
for (int cnt=0; cnt < 20; cnt++)
{
	if (history[cnt].counter > max)
	{
		max = history[cnt].counter;
		index = cnt;	
	} 
}	
return index;
}

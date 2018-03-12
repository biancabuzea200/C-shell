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
#define HISTORY_PATH ".hist_list"

typedef struct history_command
{

int counter;
char command[MAXCHAR];

}history_command;

void readInput(char **tokens,history_command* history);
void setToHome();
void setPath(char *token);
void printPath();
int execute_command(char **tokens,history_command* history);
int getNumberOfTokens(char **tokens);
void wrongNumOfTokensError(char *command);
int numberOfCommands(history_command* history);
void display_history(history_command* history);
int parseHistory(char* history_invocation,history_command* history);
int isNumber(char* string);
int lastCommand(history_command* history);
void saveHistory(history_command* history);
void loadHistory(history_command* history);


int main()
{
	
	char **input;
	history_command history[20];
	
	char *path;
	
	// Print PATH environment variable
	

	// Save the PATH to restore it later
	path = strdup(getenv("PATH")); 

	// Set the current direcotry to home
	setToHome();
	
	loadHistory(history);	
	// Read the input from the user
	readInput(input,history);

	
	saveHistory(history);
	
	// Restore the original PATH
	setPath(path);
	
	

	return 0;
}

/*
Reads, parses the user input and returns the program return status
Parameters: char tokens[MAXCHAR]
*/
void readInput(char **tokens,history_command* history)
{
	int token_cnt = 0; // number of tokens
	char input[MAXCHAR] = "";	
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	char c, *systemsymbol = "$";
	int command_cnt = history[lastCommand(history)].counter; 
	
	int index = 0; //index in the array
	
	tokens = (char **)malloc(50 * sizeof(char *));
	
	while (1)
	{		
		token_cnt = 0;
		
		printf("%s ", systemsymbol);			
		fgets(input, MAXCHAR, stdin);
			
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
		
		if(strcspn(input,"!") != 0 && strcmp(input,"\n") != 0)
			{						
							
					strcpy(history[(command_cnt) % 20].command,input);
					history[(command_cnt) % 20].counter = command_cnt + 1;
				
					command_cnt++; 	
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
	
		if (tokens[0] != NULL)
		{					
			if(execute_command(tokens,history) == -1)			
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
		
		getcwd(current_dir, 100);
		
	}
		
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
int execute_command(char **tokens,history_command* history)
{	
	pid_t pid;
	int pid_status;
	const char exit_command[6] = "exit";
	char commandNumber[3];
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

	 	if(getNumberOfTokens(tokens) > 1)
		{
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
		if(numberOfCommands(history) == 0)
		printf("There are no commands stored!\n");
		
		else if(getNumberOfTokens(tokens) > 1)
		{
			printf("Invalid number of arguments - the function expects 1 argument only!\n");
		}
		else
		display_history(history);				
	}
	else if(tokens[0][0] == '!' && tokens[0][1] != '!' && tokens[0][1] != '-')
	{	
		

		if(strlen(tokens[0]) > 3){
		printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
		return 0;
		}
		
		else if (strlen(tokens[0]) < 3)		
		{ 
		memmove(commandNumber,tokens[0]+1,1); 		
		}
		else
		{ 
		memmove(commandNumber,tokens[0]+1,2); 
		}
		
		
		if(getNumberOfTokens(tokens) != 1)
		{ 
			printf("Invalid number of arguments!, please provide a number from 1 to %d for the second argument!\n", 			numberOfCommands(history));
		}	
		else if(atoi(commandNumber) > numberOfCommands(history))
		{
		printf("Invalid number of command!,please provide a number from 1 to %d for the second argument!\n",
			numberOfCommands(history)));	
			
		}
		else if(isNumber(commandNumber) == 1 )
		{
		printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
		}		
		
		else
		{
			if (strcmp(history[atoi(commandNumber)-1].command, "\n") != 0){
	
				if(parseHistory(history[atoi(commandNumber)-1].command,history) == -1)
					return -1;
				}	
		}
	
	}



	//Executes last command in history.
	else if(tokens[0][0] == '!' && tokens[0][1] == '!')
	{	
		if(strlen(tokens[0]) != 2)
			{
				printf("Invalid format for the function !!. The function !! will excecute the last command in history \n");
				return 0;
			}

		if(getNumberOfTokens(tokens) != 1)
		{ 
			printf("Invalid number of arguments!\n");
			return 0;
		}
		else 
		{
			if(numberOfCommands(history) != 0 )
			{
				
				if (strcmp(history[lastCommand(history)].command, "\n") != 0){
				
					if(parseHistory(history[lastCommand(history)].command,history) == -1)
						return -1;	

				}
			
					
			}
			else 
			{
				printf("There are no commands saved in history!\n");
			}
		}
	}
		//Executes Last command - a number
	      else if (tokens[0][0] == '!' && tokens[0][1] == '-')
		{	
			if(strlen(tokens[0]) > 4){
				printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
				return 0;
				}
			
			else if (strlen(tokens[0]) < 4)		
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
			else if(atoi(commandNumber) > numberOfCommands(history) || atoi(commandNumber) == 0)
				{
					printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));		
				}
			else if(isNumber(commandNumber) == 1)
				{
					printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
				}
			else
			{ 
				if (strcmp(history[atoi(commandNumber)-1].command, "\n") != 0)
				{			 			
					if (lastCommand(history) - atoi(commandNumber) + 1 > 0)	{
						
									
						if(parseHistory(history[lastCommand(history)  - atoi(commandNumber) +1].command,history) == -1)
							return -1;
						

					}
					


			
					
					else if (lastCommand(history) - atoi(commandNumber) + 1 == 0){
						if(parseHistory(history[0].command,history) == -1)
						return -1;

					
					}
					else
					{	
					if(parseHistory(history[20 - (atoi(commandNumber) - lastCommand(history))+1].command,history) == -1)
					return -1;
					
						
					}
				}											
			}	
		}

	else if (tokens[0][0] == '!'){
	printf ("invalid format of argument! Either use !! or !-number \n");
	return 0;
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
void display_history (history_command* history) 
{
int history_number = 0;
int commandNumber = numberOfCommands(history);
for(history_number = 0; history_number < commandNumber; history_number++)
	{	
		printf("%d %s" ,history_number+1 ,history[history_number].command);	
	}
}


//this works 
int numberOfCommands(history_command* history)
{

int i = 0;
while(strlen(history[i].command) > 0 && i < 20)
{ 
	i++; 
}
return i;
}

/*
This parses History inputs.
*/
int parseHistory(char* history_invocation,history_command* history)
{
	int token_cnt = 0; // number of tokens
	char* history1 = strdup(history_invocation);
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	char **parsedHistory;
	parsedHistory = (char **)malloc(20 * sizeof(char *));

		token = strtok(history1, delimiters);
		
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
		if (parsedHistory[0] != NULL)
		{						
			if(execute_command(parsedHistory,history) == -1)
					return -1;
									
		}
	return 0;	
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
int lastCommand(history_command* history)
{
	int index = 0;
	int cnt = 0;
	int max = 0;
	while(history[cnt].counter != 0)
	{
	
	if (history[cnt].counter > max && cnt < 20)
	{
		max = history[cnt].counter;
		index = cnt;	
	} 
	cnt++;
	}	
	return index;
}


void loadHistory(history_command* history)
{		
	FILE *hist = fopen(HISTORY_PATH, "r");
	char input[MAXCHAR] = "";
    	char cnt[100];
	int whitespace;
	int counter;
	
	if(hist == NULL)
	{
	printf("Failed to open the file .hist_list! \n ");
	}
	else
	{	
		while(!feof(hist))
		{
			
			fscanf(hist,"%s",cnt);
			counter = atoi(cnt);
			if(counter == 0)
			{
				printf("Invalid format of .hist_list file!\n");
				hist = fopen(HISTORY_PATH,"w");
				//fclose(hist);
				break;
			}
			
			
			//fgetc used to take the space between the counter and the command
			whitespace = fgetc(hist);
			
			if(whitespace != 32 && whitespace != -1)
			{
				printf("Invalid format of .hist_list file!\n");
				hist = fopen(HISTORY_PATH,"w");
				//fclose(hist);
				break;
			}
			fgets(input,MAXCHAR,hist);		
			history[(counter-1)%20].counter = counter;
			strcpy(history[(counter-1)%20].command,input);		
		}
	
    		fclose(hist);
	}
}


void saveHistory(history_command* history)
{
	int history_number ;
	int index = numberOfCommands(history);
	setToHome();	
	FILE *hist = fopen(HISTORY_PATH,"w");
	
	if(hist == NULL)
	{
		printf("Failed to open the file .hist_list! \n ");
	}
	else
	{
		
		for (history_number=1; history_number <= index; history_number ++)
		{	
			fprintf(hist, "%d %s", history[history_number-1].counter, history[history_number-1].command);		
		}  
    		fclose(hist);
	}
   
}






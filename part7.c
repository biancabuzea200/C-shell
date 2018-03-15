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
#define ALIASES_COUNT 10
#define HISTORY_PATH ".hist_list"

typedef struct history_command
{
int counter;
char command[MAXCHAR];
}history_command;

typedef struct aliases
{
char alias[MAXCHAR];
char value[MAXCHAR];
}aliases_mapping;

void readInput(char **tokens,history_command* history,aliases_mapping* aliases);
void setToHome();
void setPath(char *token);
void printPath();
int execute_command(char **tokens,history_command* history,aliases_mapping aliases[ALIASES_COUNT]);
int getNumberOfTokens(char **tokens);
void wrongNumOfTokensError(char *command);
int numberOfCommands(history_command* history);
void display_history(history_command* history);
int parseHistory(char* history_invocation,history_command* history,aliases_mapping aliases[ALIASES_COUNT]);
int isNumber(char* string);
int lastCommand(history_command* history);
void saveHistory(history_command* history);
void loadHistory(history_command* history);
int isAlias(char* line,aliases_mapping aliases[ALIASES_COUNT]);
char* substring(char line[MAXCHAR],int index1,int index2);
int aliasesFreeIndex(aliases_mapping aliases[ALIASES_COUNT]);


int main()
{	
	char **input;
	history_command history[HISTORY_COUNT];
	aliases_mapping aliases[ALIASES_COUNT];	
	char *path;
	// Save the PATH to restore it later
	path = strdup(getenv("PATH")); 

	// Set the current direcotry to home
	setToHome();
	
	loadHistory(history);	
	// Read the input from the user
	readInput(input,history,aliases);

	saveHistory(history);
	
	// Restore the original PATH
	setPath(path);
	
	return 0;
}

/*
Reads, parses the user input and returns the program return status
Parameters: char tokens[MAXCHAR]
*/
void readInput(char **tokens,history_command* history,aliases_mapping* aliases)
{
	int token_cnt = 0; // number of tokens
	char input[MAXCHAR] = "";	
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	char c, *systemsymbol = "$";
	int command_cnt = history[lastCommand(history)].counter; 
	int index = 0; //index in the array
	char* alias;
	tokens = (char **)malloc(50 * sizeof(char *));
	
	while (1)
	{		
		token_cnt = 0;
		
		printf("%s ", systemsymbol);			
		fgets(input, MAXCHAR, stdin);
		char* inputCopy = strdup(input);	
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
		//ALIASES		

			index = strcspn(input," ");
			alias = strdup(substring(input,0,index - 1));			
			int aliasIndex = isAlias(alias,aliases);
			
			if (aliasIndex != -1)
			{	
				
			
				if(strlen(input) -1> strlen(aliases[aliasIndex].alias))
				{
				char* aliasRemainder = strdup(substring(input,index,strlen(input)));
			
		
				char* aliasCommand = strdup(aliases[aliasIndex].value);					
				strcat(aliasCommand,aliasRemainder);
				strcpy(input,aliasCommand); 
				
				}
				else
				{	
					strcpy(input,aliases[aliasIndex].value);
					strcat(input,"\n");						
				}			
			}
		//History invocations
			if(strcspn(input,"!") != 0 && strcmp(input,"\n") != 0)
			{													
					if (command_cnt < 20)					
					{
						strcpy(history[command_cnt].command,inputCopy);
						history[command_cnt].counter = command_cnt + 1;				
						command_cnt++;
					}
					else
					{
						for (int i = 0; i < 19; i++)
						{
							strcpy(history[i].command,history[i+1].command);
						}
						strcpy(history[19].command,inputCopy);
						history[command_cnt].counter = command_cnt + 1;	
						//command_cnt++;
					}
			}
	
		//Tokenization
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
	
		//Command execution
		if (tokens[0] != NULL)
		{					
			if(execute_command(tokens,history,aliases) == -1)			
			break;			
		}	
  }
}

/*
The following function sets the Environment variable HOME.
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
The following function prints the Environment variable PATH.
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
Parameters: char** tokens
	    char* initPath.
*/
int execute_command(char **tokens,history_command* history,aliases_mapping aliases[ALIASES_COUNT])
{	
	pid_t pid;
	int pid_status;
	const char exit_command[6] = "exit";
	char* commandNumber;
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
	//Displays the commands history containing the last 20 commands
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

	else if (tokens[0][0] == '!' && strlen(tokens[0]) == 1)
	{
	printf ("Invalid format of argument! Either use !! or !-number \n");
	return 0;
	}

	else if(tokens[0][0] == '!' && tokens[0][1] != '!' && tokens[0][1] != '-')
	{			
		if(strlen(tokens[0]) > 3){
		printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
		return 0;
		}
		
		else if (strlen(tokens[0]) < 3)		
		{ 
		commandNumber = substring(tokens[0],1,2); 		
		}
		else
		{ 
		commandNumber = substring(tokens[0],1,3); 
		}
				
		if(getNumberOfTokens(tokens) != 1)
		{ 
			printf("Invalid number of arguments, please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
		}	
		else if(atoi(commandNumber) > numberOfCommands(history))
		{
		printf("Please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));	
			
		}
		else if(isNumber(commandNumber) == 1 )
		{
		printf("Invalid format for the function, please provide a number from 1 to %d for the second argument!\n",numberOfCommands(history));
		}		
		
		else
		{
			if (strcmp(history[atoi(commandNumber)-1].command, "\n") != 0)
			{
				if(parseHistory(history[atoi(commandNumber)-1].command,history,aliases) == -1)
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
				
					if(parseHistory(history[lastCommand(history)].command,history,aliases) == -1)
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
				 commandNumber = substring(tokens[0],2,3); 		 		
			}
			else
			{ 				
				commandNumber = substring(tokens[0],2,4); 		  
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
						
									
						if(parseHistory(history[lastCommand(history)  - atoi(commandNumber) +1].command,history,aliases) == -1)
							return -1;
					}
					else if (lastCommand(history) - atoi(commandNumber) + 1 == 0){
						if(parseHistory(history[0].command,history,aliases) == -1)
						return -1;
					}
					else
					{	
					if(parseHistory(history[20 - (atoi(commandNumber) - lastCommand(history))+1].command,history,aliases) == -1)
					return -1;	
					}
				}											
			}	
		}

	else if(strcmp(tokens[0],"alias") == 0)
	{
		int freeIndex = aliasesFreeIndex(aliases);
				
		char command[MAXCHAR] = "";

		if(getNumberOfTokens(tokens) < 3)
		{
		printf("Invalid number of arguments for function alias!\n");
		}
		else if(isAlias(tokens[1],aliases) != -1)
		{	
			int aliasIndex = isAlias(tokens[1],aliases);
			if(strlen(tokens[1]) != 1)
			printf("Overriding alias '%s'!\n",tokens[1]);

			for (int i = 2; i < getNumberOfTokens(tokens); i++)
			{
				strcat(command,tokens[i]);
				strcat(command," ");
			}
			strcpy(aliases[aliasIndex].alias,tokens[1]);
			strcpy(aliases[aliasIndex].value,command);
		}		
		else if(freeIndex == -1)
		{
		printf("The aliases array is full! Please remove an alias before adding more.\n");		
		}	
		else if (isAlias(tokens[1],aliases) == -1)
		{
			for (int i = 2; i < getNumberOfTokens(tokens); i++)
			{
				strcat(command,tokens[i]);
				strcat(command," ");
			}
			strcpy(aliases[freeIndex].alias,tokens[1]);
			strcpy(aliases[freeIndex].value,command);
		}		
	}
	
	else if(strcmp(tokens[0],"unalias") == 0 )
	{
		if(getNumberOfTokens(tokens) != 2)
		{
		printf("The function 'unalias' expects 2 arguments!\n");
		}
	
		else if(isAlias(tokens[1],aliases) != -1)
		{
		strcpy(aliases[isAlias(tokens[1],aliases)].alias,"");
		strcpy(aliases[isAlias(tokens[1],aliases)].value,"");
		}
		
		else if(isAlias(tokens[1],aliases) == -1)
		{
		 printf("There is no alias %s saved !\n",tokens[1]);
		}	
	}
	
	else if(strcmp(tokens[0],"aliases") == 0)
	{
		int free = aliasesFreeIndex(aliases);
		if (getNumberOfTokens(tokens) != 1)
		{
			printf("The function 'aliases' expects only 1 argument!\n");
		}
		else if(free == 0)
		{
			printf("There are no aliases saved!\n");
		}
		else 
		{
			for(int i = 1; i<=10 ;i++)

			{
				if(strcmp(aliases[i-1].alias,"") == 0)				
				continue;
				
				printf("%d.%s\n",i,aliases[i-1].alias);	
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

void display_history (history_command* history) 
{
int history_number = 0;
int commandNumber = numberOfCommands(history);
for(history_number = 0; history_number < commandNumber; history_number++)
	{	
		printf("%d %s" ,history_number+1 ,history[history_number].command);	
	}
}


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
Function used to parse history invocations.
Parameters: char* history_invocation
            history_command* history
	    aliases_mapping aliases[ALIASES_COUNT]
*/
int parseHistory(char* history_invocation,history_command* history,aliases_mapping aliases[ALIASES_COUNT])
{
	int token_cnt = 0; // number of tokens
	char* history1 = strdup(history_invocation);
	const char delimiters[13] = " \t<>|;&\n";
	char *token = "";
	char **parsedHistory;
	parsedHistory = (char **)malloc(20 * sizeof(char *));
			//Replace aliases
		
			int index = strcspn(history_invocation," ");
			char *alias = strdup(substring(history1,0,index - 1));		
			int aliasIndex = isAlias(alias,aliases);
			
			
			if (aliasIndex != -1)
			{	
				
			
				if(strlen(history1) -1 > strlen(aliases[aliasIndex].value))
				{
				char* aliasRemainder = strdup(substring(history1,index,strlen(history1)));
				
		
				char* aliasCommand = strdup(aliases[aliasIndex].value);					
				strcat(aliasCommand,aliasRemainder);
				strcpy(history1,aliasCommand); 
				
				}
				else
				{	
				
					strcpy(history1,aliases[aliasIndex].value);
					strcat(history1,"\n");
					
						
				}			
			}

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
			if(execute_command(parsedHistory,history,aliases) == -1)
					return -1;
									
		}
	return 0;	
}

/*
Checks if a given string is a number.
Parameters: char* string
*/
int isNumber(char* string)
{

int length = strlen(string);

for (int j = 0; j<length-1; j++)
{
	if (!isdigit(string[j]))
		return 1;
}
return 0;
}

/*
Returns the index of the last command in history array.
Parameters: history_command* history
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
	printf("Failed to open the file .hist_list!\n");
	}
	else
	{	
		//int i = -1;
		while(!feof(hist))
		{
			//i++;
			fscanf(hist,"%s",cnt);
			counter = atoi(cnt);
			if(counter == 0)
			{
				printf("Invalid format of .hist_list file!\n");
				hist = fopen(HISTORY_PATH,"w");
				//history = malloc(sizeof(history_command)*20);
				break;
			}
			
			
			//fgetc used to take the space between the counter and the command
			whitespace = fgetc(hist);
			
			if(whitespace != 32 && whitespace != -1)
			{
				printf("Invalid format of .hist_list file!\n");
				hist = fopen(HISTORY_PATH,"w");
				//history = malloc(sizeof(history_command)*20);
				break;
			}
			fgets(input,MAXCHAR,hist);		
			history[(counter-1)%20].counter = counter;
			strcpy(history[(counter-1)%20].command,input);		
		}
	
    		fclose(hist);
	}
}
/*


*/

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
/*
Takes a string and checks if it is an alias.The function returns the index in the aliases array if the string is an alias or -1 if it is not an alias.
Parameters char* line
           aliases_mapping aliases[ALIASES_COUNT]
*/

int isAlias(char* line,aliases_mapping aliases[ALIASES_COUNT])
{
	for (int index = 0; index < ALIASES_COUNT; index++)
	{
		if (aliases[index].alias != NULL)
		{		
			if(strcmp(aliases[index].alias,line) == 0 || strcmp(aliases[index].alias,substring(line,0,strlen(line)-2)) == 0)
			{	
				return index;
			}
		}	
	}
return -1;
}

/*
Takes in a string and 2 indexes and returns the substring between index1 and index 2.
Parameters: int index1,index2
            char line[MAXCHAR]		
*/
char* substring(char line[MAXCHAR],int index1,int index2)
{
int cnt = 0;
char *substring = (char *)malloc(MAXCHAR * sizeof(char *));
	memmove(substring,line + index1,index2-index1+1);	
return substring;
}

/*
Returns the index of the next free spot in the aliases array.If the array is full, the function returns -1.
Parameters: aliases_mapping aliases[ALIASES_COUNT]
*/
int aliasesFreeIndex(aliases_mapping aliases[ALIASES_COUNT])
{
int index = 0;	
	for(int i = 0;i < ALIASES_COUNT; i++)
	{
		if(strcmp(aliases[i].alias,"") == 0)
		{
			return index;
		}
	index ++;
	}
	//returns -1 if the array is full
	return -1;
}

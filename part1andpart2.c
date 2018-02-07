#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAXCHAR 512


char** readInput(char** tokens);
void shakeItOff(char** argv, char* path);
int getNumberOfTokens(char** tokens);

int main () 
{
char** input;
char path[100];
strcpy(path,"/bin/");
input = readInput(input);

	//Tests if the tokens have been stored correctly... 
	printf("%s\n", input[0]);



//strcat(PATH,input[0]);
printf("PATH IS -> %s\n", getenv("PATH"));
//printf("NUMMBER OF TOKENS IS: %d\n",getNumberOfTokens(input));
shakeItOff(input,path);		
return 0;
}

/*
Reads,parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
char** readInput(char** tokens)
{
int cnt = 0;
char input[MAXCHAR] = "";
const char d[13] = " \t<>|;&\n";
char *token = "";
const char check[6] = "exit\n";
char c,systemsymbol = '$';
   
	tokens = (char**) malloc(50*sizeof(char*));
	for (int i = 0; i < 50; i++)
	{
   		tokens[i] = (char*) malloc(MAXCHAR*sizeof(char));
	}

	printf("Array successfully created!\n");

while(1)
{
	printf("%c",systemsymbol);

	fgets(input,MAXCHAR,stdin);
	//Checks if "exit" or Ctrl + D are present in the input...
        if((strcmp(check,input) == 0) || feof(stdin) )
	{
        	break;
        }
	//Checks if the input is more than 512 symbols...        
	if(input[strlen(input)-1] != '\n')
	{
        	printf("%cThe command is too long!\n",systemsymbol);  
		//The following line flushes the buffer after the 512th symbol...
		while ((c = getchar()) != '\n' && c != EOF) {}
        	strcpy(input,"");
        }

	token = strtok(input, d);
      
	while( token != NULL )
	{
		if (strcmp(token,"\n") != 0)
		{
			strcpy(tokens[cnt],token);
			printf("COPYING %s into tokens[%d]\n",token,cnt);
			cnt++;
		}
      	token = strtok(NULL, d);
      	}
}
return tokens;
}

void shakeItOff(char** argv, char* path)
{
pid_t pid;
//fork a child process
pid = fork();

strcat(path,argv[0]);

if(pid < 0) 
{ 
	//if error occurs	
	fprintf(stderr,"Fork Failed");
	exit(-1);
}
else if (pid == 0)
{
	printf("sadsdsd\n");
		//child process
		execv(path,argv);
}
else
	{
		//parent process
		wait(NULL);
		printf("Child Complete\n");
		exit(0);
	}


}


int getNumberOfTokens(char** tokens)
{
int i = 0,cnt = 0;

while (strlen(tokens[i]) > 0)
{
	i++;
	cnt++;
}
return cnt;
}


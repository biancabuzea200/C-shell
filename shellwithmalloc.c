#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCHAR 512

char** readInput(char** tokens);

//char** TwoDimCharArray(int arraySizeX, int arraySizeY);

int main () 
{
char** input;

input = readInput(input);

//Tests if the tokens have been stored correctly... 

	printf("%s\n", input[0]);
 		
return 0;
}

/*
Reads,parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
char** readInput(char** tokens)
{

tokens = (char**) malloc(50*sizeof(char*));
	for (int i = 0; i < 50; i++)
	{
   		tokens[i] = (char*) malloc(MAXCHAR*sizeof(char));
	}

printf("array successfully created!");

    int cnt = 0;
    char input[MAXCHAR] = "";
    const char d[13] = " \t<>|;&\n";
    char *token = "";
    const char check[6] = "exit\n";
    char c,systemsymbol = '$';
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
	//printf("%s\n", token );
	}
      token = strtok(NULL, d);
      }
   }
return tokens;
}


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCHAR 512
char* readInput(char tokens[MAXCHAR]);

int main () 
{
char input[512] = "";

//Tests if the tokens have been stored correctly... 
	
		printf("%s",readInput(input));

return 0;
}

/*
Reads,parses the user input and returns a char pointer to the tokens array.
Parameters: char tokens[MAXCHAR]
*/
char* readInput(char tokens[MAXCHAR]){

    char input[MAXCHAR] = "";
    const char d[13] = " \t<>|;&\n";
    char *token = "";
    const char check[6] = "exit\n";
    char c,systemsymbol = '$';
   while(1)
   {
      printf("%c",systemsymbol);

<<<<<<< HEAD
      fgets(input,512,stdin);
        if(strcmp(check,input) == 0 | feof(stdin)){
=======
      fgets(input,MAXCHAR,stdin);
	//Checks if "exit" or Ctrl + D are present in the input...
        if((strcmp(check,input) == 0) || feof(stdin) )
	{
>>>>>>> 7ee70f2e947fbc4ff6288ddea6ceae374141c9de
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
		
			strcat(tokens,token);
		
	printf("%s\n", token );
	}
      token = strtok(NULL, d);
      }
   }
return tokens;
}


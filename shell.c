#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void readInput();

int main () {
readInput();
return 0;
}

/*
Reads and parses the user input in tokens.
*/
void readInput(){

    char input[512] = "";
    const char d[12] = " \t<>|;&";
    char *token = "";
    const char check[6] = "exit\n";
   while(1)
   {
      printf("$");

      fgets(input,512,stdin);
        if(strcmp(check,input) == 0 | feof(stdin)){
            break;
        }
        if(input[strlen(input)-1]!= '\n'){
            printf("The command is too long!\n");
            input[0] = "";
            }

      token = strtok(input, d);

      while( token != NULL )
      {
        printf("%s\n", token );
        token = strtok(NULL, d);
      }
   }
}


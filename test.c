#include <string.h>
#include <stdio.h>

int main () {
   char input[512] = "";
   const char d[2] = " ";
   char *token = "";
   const char check[5] = "exit";
   while(strcmp(check,input) != 0)
   {
      gets(input);
      token = strtok(input, d);
      while( token != NULL )
      {
         printf("%s\n", token );
         token = strtok(NULL, d);
      }
   }
   return 0;
}

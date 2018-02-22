#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAXCHAR 512
#define MAX_COMMAND_LENGHT 128
#define HISTORY_COUNT 20
#define HISTORY_PATH ".hist_list"

void readInput(char **tokens);
void setToHome();
void setPath(char *token);
void printPath();
int execute_command(char **tokens);
int getNumberOfTokens(char **tokens);
void wrongNumOfTokensError(char *command);
void loadHistory();
void saveHistory();
void printHistory();
char history[20][MAXCHAR];
int history_next;

int main()
{
    char **input;
    char *path;

    history_next = 0;
    loadHistory();
    //printHistory();

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

    return 0;
}

/*
Loads the history from .hist_list into the history data structure.
*/
void loadHistory()
{
    FILE *hist = fopen(HISTORY_PATH, "r");
    int i = 19;
    while(!feof(hist))
    {
      fgets(history[i], MAXCHAR, hist);
      i--;
    }
    fclose(hist);
}

/*
Saves the history data structure to .hist_list
*/
void saveHistory()
{
    FILE *hist = fopen(HISTORY_PATH, "w");
    for(int i = history_next-1; i>=0; i--)
    {
        if(strcmp(history[i], "") == 0) continue;
        fprintf(hist, "%s", history[i]);
    }
    for(int i = 19; i>=history_next; i--)
    {
      if(strcmp(history[i], "") == 0) continue;
      fprintf(hist, "%s", history[i]);
    }
    fclose(hist);
    printf("History saved!\n");
}

/*
Prints the contents of the history data structure.
*/
void printHistory()
{
    int j = 1;
    for(int i = history_next-1; i>=0; i--)
    {
        if(strcmp(history[i], "") == 0) continue;
        printf("%d %s", j, history[i]);
        j++;
    }
    for(int i = 19; i>=history_next; i--)
    {
      if(strcmp(history[i], "") == 0) continue;
      printf("%d %s", j, history[i]);
      j++;
    }
}

/*
Saves cmd to history.
*/
void saveCmd(char *cmd)
{
  memcpy(history[history_next], cmd, MAXCHAR);
  history_next++;
  if(history_next == 20)
  {
    history_next = 0;
  }
}

/*
Reads, parses the user input and returns the program return status
Parameters: char tokens[MAXCHAR]
*/
void readInput(char **tokens)
{
    int token_cnt = 0;
    char input[MAXCHAR] = "";
    const char delimiters[13] = " \t<>|;&\n";
    char *token = "";

    char c, *systemsymbol = "$ ";

    tokens = (char **)malloc(50 * sizeof(char *));
    printf("Tokens array successfully created!\n");

    while (1)
    {
        token_cnt = 0;
        printf("%s", systemsymbol);

        fgets(input, MAXCHAR, stdin);

        saveCmd(input);

        if (feof(stdin)) {
            printf("\n");
            saveHistory();
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

        if (tokens[0] != NULL)
        {
            if(execute_command(tokens)== -1)
            {
                break;
            }
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
    else if(strcmp(tokens[0], "history") == 0)
    {
      printf("History\n");
      printHistory();
    }
    else if(strcmp(tokens[0], "next") == 0)
    {
      printf("%d/n", history_next);
    }
    else if(strcmp(exit_command, tokens[0]) == 0)
    {
        saveHistory();
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
            while (wait(&pid_status) != pid)
                ;
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

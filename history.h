
#define MAXCHAR 512 

#define HISTORY_COUNT 20




typedef struct history_command{

char* command[MAXCHAR] ;
int counter; // 1 to infinity
}history_command;

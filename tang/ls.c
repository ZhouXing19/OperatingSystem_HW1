#include <limits.h>
#include <stdio.h>
#include <stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 
#include<sys/wait.h>

# define MAX_INPUT_LEN 64

/* singleCommand process a single command.
 * case 1: built-in command -> execBuiltInCmd()
 * case 2: system command -> 
 * case 2-1: non-'ls' command -> parse and execvp()
 * case 2-2-1: 'ls' command without redirection'>'
 * case 2-2-2: 'ls' command with redirection '>
 * */
int singleCommand(char* cmd){
  char* parsed[20];
  int i=0;
  parsed[i] = strtok(cmd, " ");
  while(parsed[i] != NULL){
    parsed[++i] = strtok(NULL, " ");
  }
  // 检查 cmd和 arg格式是不是正确
  if (execvp(parsed[0], parsed) < 0) { 
    printf("\nCould not execute command.."); 
  }  
  return 0;
}

int getUserInput(char* userInput){
  char buf[MAX_INPUT_LEN]; 
  fgets(buf, MAX_INPUT_LEN, stdin);
  char *token = strtok(buf, "\n");
  if (strlen(token)>MAX_INPUT_LEN){
    // error
    return 1;
  }
  strcpy(userInput, buf);
  return 0;
}

int main(){
  char *cmd;
  getUserInput(cmd);
  int cpid = fork();
  if(cpid == 0){
    singleCommand(cmd);
  }
  else{
    wait(NULL);
  }
  return 0;
}
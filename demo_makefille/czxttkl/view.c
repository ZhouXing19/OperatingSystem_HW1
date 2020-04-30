#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 

void printError(){
  char error_message[30] = "An ERROR has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
  exit(0);
  // TODO: exit gracefully: process or program, where to put exit(0) -> Notes 1
  // exit from the shell？？
  // for extra args, you should print an error message to stderr and then exit gracefully.
}

int execRedirectCmd(char* userInput){
    char* cmd = strtok(userInput, ">");
    char* output_fd = strtok(NULL, " ");
    // check invalid input eg. ls > out1 out2
    char* extra = strtok(NULL, " ");
    if(extra == NULL) printError();

    int fd1 = open("output_fd", O_RDWR); 
    if (fd1 < 0) exit(1); // TODO: exit(0 or 1)
    // separate cmd part
    int p=0;
    char* parsed[20];
    char* token = strtok(cmd, " ");
    while(token != NULL){
      parsed[p] = token;
      token = strtok(NULL, " ");
      p++;
    }
    // set output file
    dup2(fd1, STDOUT_FILENO);

    // execute cmd
    execvp(parsed[0], parsed);

    // Using close system Call 
    close(fd1); 
    return 0;
}

int main(int argn, char* args[]){
switch (argn)
{
case 2:
	/* code */
	printf("yes!");
	break;

default:
	break;
}

}
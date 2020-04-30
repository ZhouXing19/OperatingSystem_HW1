#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 

#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

# define MAX_INPUT_LEN 64

// STEP 4 Syntax Error *********************************************************
void printError(){
  char error_message[30] = "An ERROR has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
  exit(0);
  // TODO: exit gracefully: process or program, where to put exit(0) -> Notes 1
  // exit from the shell？？
  // for extra args, you should print an error message to stderr and then exit gracefully.
  // exit or return;
}

int execRedirectCmd(char* userInput, int* input){
    // TODO ================================================
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

int execSimpleCmd(char* userInput, int* output, int* input){
  // check if dup2 is needed
  if (input != NULL) dup2(*input, STDIN_FILENO);
  if (output != NULL) dup2(*output, STDOUT_FILENO);
  // copy user input for echo parsing
  char copyInput[64];
  strcpy (copyInput, userInput);
  int quote = (userInput[strlen(userInput)-1] == '\"')? 1: 0;

  // general parsing
  char* parsed[20];
  int i=0;
  parsed[i] = strtok(userInput, " ");
  while(parsed[i] != NULL){
    parsed[++i] = strtok(NULL, " ");
  }

  if(strcmp(parsed[0],"bye") == 0){
    if(parsed[1]==NULL) exit(0); // exit successfully
    printError(); // extra arg error
  }
  
  else if(strcmp(parsed[0],"cd")==0){
    if (parsed[2]!= NULL)  printError(); // extra arg error
    int res = parsed[1] == NULL? chdir(getenv("HOME")): chdir(parsed[1]);
  }
  
  else if(strcmp(parsed[0],"echo")==0){
    // case 1: no args
    if(parsed[1] == NULL){
      printf("\n");
    } 
    // case 2: arg starts with '\' 
    else if(parsed[1][0] == '\''){
      printError();
    }
    // case 3: arg starts with " 
    else if(parsed[1][0] == '"'){
      char* cmd = strtok(copyInput, "\"");
      char* str = strtok(NULL, "\""); 
      char* rest = strtok(NULL, "\"");
      if(rest != NULL || quote == 1){
        printf("%s\n", str);
      }
      else{
        // without trailing "
        printError();
      }
    }
    // case 4: normal case: echo arg
    else{
      printf("%s\n", parsed[1]);
    }
  }

  else if(strcmp(parsed[0],"pwd")==0){
    if(parsed[1] != NULL) printError(); // extra arg error
    char cwd[1024]; 
	  getcwd(cwd, strlen(cwd)); 
	  printf("%s\n", cwd);
  }

  // system function:
  else{
    // printf("System function --- cmd is %s\n", parsed[0]);
    execvp(parsed[0], parsed);
    // printf("finish");
  }

  //
  if(input != NULL) close(*input);
  if(output != NULL) close(*output);
  return 0;
}

// STEP 2 **********************************************************************
int execNonPipeCmd(char* userInput, int* output, int* input){
  char* redirectSymbol = strchr(userInput, '>');
  if (redirectSymbol != NULL) execRedirectCmd(userInput, input);
  else{
    execSimpleCmd(userInput, output, input);
  }
  return 0;
}

int execPipeCmd(char* userInput){
  char* parsed[20];
  int n=0;
  parsed[n] = strtok(userInput, "|");
  while(parsed[n] != NULL){
    parsed[++n] = strtok(NULL, "|");
  }
  // printf("We have %d commands\n", n);

  int p[2];
  int pid;
  int status;

  pipe(p);
  // write(p[1], " ", 64);
  for(int i=0; i<n; i++){
    // redirect
    if((pid = fork())== 0){
      if(i != n-1){
        // dup2(*p, STDOUT_FILENO);
        execNonPipeCmd(parsed[i], &p[1], &p[0]);
        exit(0);
      }
      else{
        execNonPipeCmd(parsed[i], NULL, &p[0]);
        exit(0);
      }
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      if(i == n-1) close(p[0]);
      close(p[1]);
      // char str[64];
      // sprintf(str, "%d", p[1]);
      // strcat(parsed[i+1], str);
      // check status, if error -> exit 
      // printf("Return child %d\n", cpid);
    }

  }
  return 0;
}


int parseSingleCommand(char* userInput){
  // reverse + strchr
  int pipe = 0;
  int redirect = 0;
  for(int i=0; i<strlen(userInput); i++){
    if(userInput[i] == '|') pipe = i;
    if(userInput[i] == '>') redirect = i;
  }
  if(pipe == 0){
    execNonPipeCmd(userInput, NULL, NULL);
  } 
  else if(redirect == 0 || redirect > pipe){
    execPipeCmd(userInput);
  } 
  else{
    printError();// error: '>' before last '|'
  }
  return 0;
}


int getUserInput(char* userInput){
  char buf[MAX_INPUT_LEN]; 
  fgets(buf, MAX_INPUT_LEN, stdin);
  char *token = strtok(buf, "\n");
  strcpy(userInput, buf);
  return 0;
}


int main(){
  char userInput[MAX_INPUT_LEN];
  printf("520shell>");
  getUserInput(userInput);
  parseSingleCommand(userInput);
}
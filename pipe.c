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

int execPipeCmd(char* userInput){
  char* parsed[20];
  int n=0;
  parsed[n] = strtok(userInput, "|");
  while(parsed[n] != NULL){
    parsed[++n] = strtok(NULL, "|");
  }

  int p[2], pid;
  if(pipe(p)<0) exit(1);

  // sequential way
  for(int i=0; i<n; i++){
    // redirect
    

  }


  return 0;
}

int execSimpleCmd(char* userInput){
  char copyInput[64];
  strcpy (copyInput, userInput);
  int quote = (userInput[strlen(userInput)-1] == '\"')? 1: 0;
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
      return 0;
    } 
    // case 2: arg starts with '\' 
    if(parsed[1][0] == '\''){
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
    if(parsed[1] == NULL) printError(); // extra arg error
    char cwd[1024]; 
	  getcwd(cwd, sizeof(cwd)); 
	  printf("%s\n", cwd);
  }

  // system function:
  else{
    printf("System function - - cmd is %s\n", parsed[0]);
    int res = execvp(parsed[0], parsed);
    printf("%s result is %d\n", parsed[0],res);
  }
    return 0;
}

// STEP 2 **********************************************************************

int parseSingleCommand(char* userInput){
  // case 1: pipeline
  // case 2: redirection
  // case 3: build-in or system function
  // TODO: fork()可能要放在这里
  int pipeSymbol = 0;
  int redirectSymbol = 0;
  for(int i=0; i<sizeof(userInput); i++){
    if(userInput[i] == '|') pipeSymbol = i;
    if(userInput[i] == '>') redirectSymbol = i;
  }
  if(pipeSymbol > 0 && redirectSymbol > pipeSymbol) execPipeCmd(userInput);
  else if(pipeSymbol == 0 &&  redirectSymbol>0) execRedirectCmd(userInput);
  else if(pipeSymbol == 0 && redirectSymbol == 0) execSimpleCmd(userInput);
  else{
    // error: '>' before last '|'
    printError();
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
  execPipeCmd(userInput);
}
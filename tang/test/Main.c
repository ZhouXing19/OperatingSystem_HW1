#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 

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

// STEP 3 **********************************************************************
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
        if(str == NULL) str = "";
        printf("%s\n", str);
      }
      else{
        printError(); // without trailing "
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
    printf("result is %d\n", res);
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

int parseSequentialCommand(char* userInput){
  printf("--- Sequential Command\n");
  int status;
  pid_t pid;
  // sequential way
  char* token = strtok(userInput, ";");
  while(token != NULL){
    if((pid = fork())== 0){
      parseSingleCommand(token);
      exit(0);
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      printf("Return child %d\n", cpid);
    }
    token = strtok(NULL, ";");
  }
  return 0;
}

int parseParallelCommand(char* userInput){
  printf("--- Parallel Command\n");
  int i, status;
  pid_t pid;
  char* token = strtok(userInput, "&");
  while(token != NULL){
    if((pid = fork())== 0){
      parseSingleCommand(token);
      exit(0);
    }
    token = strtok(NULL, "&");
  }

  while (wait(NULL) != -1){
    pid_t cpid = waitpid(pid, &status, 0);
    printf("Return child %d\n", cpid);
  }
  return 0;
}


// STEP 1 **********************************************************************
int getUserInput(char* userInput){
  char buf[MAX_INPUT_LEN]; 
  fgets(buf, MAX_INPUT_LEN, stdin);
  char *token = strtok(buf, "\n");
  if (strlen(token)>MAX_INPUT_LEN) printError();
  strcpy(userInput, buf);
  return 0;
}

int checkMixingCommand(char* userInput){
  int hasAnd;
  int hasSemicolon;
  int i;

  hasAnd = 0;
  hasSemicolon = 0;

  for(i=0; i<sizeof(userInput); ++i){
    // printf("char is %c\n", userInput[i]);
    if(userInput[i] == '&'){
      hasAnd = 1;
    }
    if(userInput[i] == ';'){
      hasSemicolon = 1;
    }
  }

  if (hasAnd == 0 && hasSemicolon == 0){
    // single command mode
    return 1;
  }
  else if (hasAnd == 1 && hasSemicolon == 0){
    // parallel mode
    return 2;
  }
  else if(hasAnd == 0 && hasSemicolon == 1){
    // sequential mode
    return 3;
  }
  else{
    // error
    return 0;
  }

}

int main(){
  // print current working directory
  // get user input
  // parse user input && check invalid input

  // WHILE LOOP
  char userInput[MAX_INPUT_LEN];
  int commandType;

  //TODO: put while loop here, change prompt based on chdir ===================
  printf("520shell>");
  getUserInput(userInput);

  commandType = checkMixingCommand(userInput);
  switch (commandType) {
    case 1:
      parseSingleCommand(userInput);
      break;
    case 2:
      parseParallelCommand(userInput);
      break;
    case 3:
      parseSequentialCommand(userInput);
      break;
    default:
      // TODO: error handling for invalid command
      printf("mixed command error");
      break;
  }
  return 0;
}
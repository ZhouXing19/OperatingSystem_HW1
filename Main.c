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

// STEP 4 **********************************************************************
int execBuitInCmd(char* userInput){

}

int execRedirectCmd(char* userInput){
    char* cmd = strtok(userInput, ">");
    char* output_fd = strtok(NULL, " ");
    // check invalid input eg. ls > out1 out2
    char* extra = strtok(NULL, " ");
    if(extra == NULL){
     //error
     return 1;
    }
    int fd1 = open("output_fd", O_RDWR); 
    if (fd1 < 0)  { 
        perror("Fail to open/ create file"); 
        exit(1); 
    }
    // separate cmd part
    int p=0;
    char* parsed[20];
    char* token = strtok(cmd, " ");
    while(token != NULL){
      parsed[p] = token;
      token = strtok(cmd, " ");
      p++;
    }
    // set output file
    dup2(fd1, STDOUT_FILENO);

    // execute cmd
    if (execvp(parsed[0], parsed) < 0) { 
      printf("\nCould not execute command.."); 
    }

    // Using close system Call 
    if (close(fd1) < 0)  
    { 
        perror("Fail to open/ create file"); 
        exit(1); 
    } 
}

int execSystemCmd(char* userInput){
   
}


// STEP 3 **********************************************************************
int execPipeCmd(char* userInput){

}

int execNonPipeCmd(char* userInput){

}
// STEP 2 **********************************************************************
int parseSingleCommand(char* userInput){
  // case 1: pipeline
  // case 2: no pipeline
  int pipeSymbol = sizeof(userInput);
  int redirectSymbol = sizeof(userInput);
  for(int i=0; i<sizeof(userInput); i++){
    if(userInput[i] == '|') pipeSymbol = i;
    if(userInput[i] == '>') redirectSymbol = i;
  }
  // non-pipe command
  if(pipeSymbol==sizeof(userInput)) execNonPipeCmd(userInput);
  else if(pipeSymbol < redirectSymbol) execPipeCmd(userInput);
  else{
    // error: '>' before last '|'
  }
  
  return 0;
}

int parseSequentialCommand(char* userInput){
  printf("--- Sequential Command\n");
  int i, status;
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
  if (strlen(token)>MAX_INPUT_LEN){
    // error function
    return 1;
  }
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
  //print current working directory
  // get user input
  // parse user input && check invalid input

  // WHILE LOOP
  char userInput[MAX_INPUT_LEN];
  int commandType;
  printf("520shell>");
  getUserInput(userInput);
  printf("CHECK USER INPUT: %s\n", userInput);

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
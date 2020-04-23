#include <limits.h>
#include <stdio.h>
#include <stdio.h>
# include<string.h> 
# include<stdlib.h> 
# include <stdbool.h>
# include<unistd.h> 
# include<readline/readline.h> 

# define MAX_INPUT_LEN 64

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

int parseSingleCommand(char* userInput){
  // case 1: pipeline
  // case 2: no pipeline
  char *token = strtok(userInput, " ");
  char *command;
  strcpy(command, token);
  
  return 0;
}

int parseSequentialCommand(char* userInput){
  printf("Sequential\n");
  return 0;
}

int parseParallelCommand(char* userInput){
  printf("Parallel\n");
  return 0;
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
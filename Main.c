#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 

# define MAX_INPUT_LEN 64

// STEP 4 Syntax Error *********************************************************
void printError(){
  char error_message[30] = "An ERROR has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
  exit(0);
}

int execSingleCmd(char* userInput){
  if(strchr(userInput, '>') != NULL){
      userInput = strtok(userInput, ">");
      char* output_file = strtok(NULL, " ");
      int fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC); 
      if (fd < 0) return 1;
      dup2(fd, STDOUT_FILENO);
    }

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
      if(parsed[1]==NULL){
        exit(0); // exit successfully
      } 
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
      else if(parsed[1][0] == '\\'){
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
	    getcwd(cwd, sizeof(cwd)); 
	    printf("%s\n", cwd);
    }

    // system function:
    else{
      execvp(parsed[0], parsed);
    }
  return 0;
}

// STEP 3 **********************************************************************
int pipefork(char* userInput, bool inPipe, bool first, bool last, int input){
  int p[2];
  int status;
  if (inPipe) pipe(p);
  pid_t pid = fork();

  if(pid == 0){
    if(inPipe && first){
      dup2(p[1], STDOUT_FILENO);
    }
    else if(inPipe && last){
      dup2(input, STDIN_FILENO);
    }
    else if(inPipe){
      dup2(p[1], STDOUT_FILENO);
      dup2(input, STDIN_FILENO);
    }
    execSingleCmd(userInput);
    exit(0);
  }
  else{
    int cpid = waitpid(pid, &status, 0);
  }

  if(inPipe){
    if(input != 0) close(input);
    close(p[1]);
    if(last) close(p[0]);
    return p[0];
  }
  return 0;
}

int execPipeCmd(char* userInput){
  int saved_stdin = dup(0);
  int saved_stdout = dup(1);
  int input = 0;

  char* parsed[20];
  int n=0;
  parsed[n] = strtok(userInput, "|");
  while(parsed[n] != NULL){
    parsed[++n] = strtok(NULL, "|");
  }


  for(int i=0; i<n; i++){
    if(i == 0){
      input = pipefork(parsed[i], true, true, false, input);
    }
    else if(i == n-1){
      input = pipefork(parsed[i], true, false, true, input);
    }
    else{
      input = pipefork(parsed[i], true, false, false, input);
    }
  }

  for(int i=0; i<n; i++){
    wait(NULL);
  }

  dup2(saved_stdout, 1);
  dup2(saved_stdin, 0);
  close(saved_stdout);
  close(saved_stdin);
  return 0;
}

int parseSingleCommand(char* userInput){
  int pipe = -1;
  int redirect = -1;
  for(int i=0; i<strlen(userInput); i++){
    if(userInput[i] == '|') pipe = i;
    if(userInput[i] == '>') redirect = i;
  }
  if(pipe == -1){
    execSingleCmd(userInput);
  } 
  else if(redirect == -1 || redirect > pipe){
    execPipeCmd(userInput);
  } 
  else{
    printError();// error: '>' before last '|'
  }
  return 0;
}

int parseSequentialCommand(char* userInput){
  int status;
  pid_t pid;
  // sequential way
  char* token = strtok(userInput, ";");
  while(token != NULL){
    pid = fork();
    if(pid < 0) return 1;
    else if(pid == 0){
      parseSingleCommand(token);
      exit(0);
    }
    else{
      int cpid = waitpid(pid, &status, 0);
    }
    token = strtok(NULL, ";");
  }
  return 0;
}

int parseParallelCommand(char* userInput){
  // printf("--- Parallel Command\n");
  int i, status;
  pid_t pid;
  char* token = strtok(userInput, "&");
  while(token != NULL){
    pid = fork();
    if(pid < 0) return 1;
    if( pid == 0){
      parseSingleCommand(token);
      exit(0);
    }
    token = strtok(NULL, "&");
  }

  while (wait(NULL) != -1){
    pid_t cpid = waitpid(pid, &status, 0);
  }
  return 0;
}


// STEP 1 **********************************************************************
int getUserInput(char* userInput){
  char buf[MAX_INPUT_LEN]; 
  fgets(buf, MAX_INPUT_LEN, stdin);
  char *token = strtok(buf, "\n");
  if (strlen(token) == MAX_INPUT_LEN) printError();
  strcpy(userInput, buf);
  return 0;
}

int checkMixingCommand(char* userInput){
  int status;
  pid_t pid = fork();
  if(pid < 0) return 1;
  else if(pid == 0){
    const char and = '&';
    const char semi = ';';
    char *findAnd = strchr(userInput, and);
    char *findSemi = strchr(userInput, semi);

    if(findAnd == NULL && findSemi == NULL){
      parseSingleCommand(userInput);
    } 
    else if(findAnd == NULL && findSemi != NULL){
      parseSequentialCommand(userInput);
    }
    else if(findAnd != NULL && findSemi == NULL){
      parseParallelCommand(userInput);
    }
    else{
      printError();
    }
  }
  else{
    int cpid1 = waitpid(pid, &status, 0);
  }
  return 0;
}

int main(int argc, char** argv){
  char userInput[MAX_INPUT_LEN];
  int commandType;

  FILE *file = NULL;
  char buffer[MAX_INPUT_LEN];

  // interactive mode
  if(argc == 1){
    while(strcmp(userInput,"bye") != 0){
      printf("520shell>");
      getUserInput(userInput);
      checkMixingCommand(userInput);
      printf("last user input is %s \n", userInput);
      // userInput = strtok(userInput, " ");
    }
  }

  // batch mode
  else if(argc == 2){
    char *bFile= strdup(argv[1]);
    if((file = fopen(bFile, "r")) == NULL){
      printError();
      return 0;
    }

    while(fgets(buffer, MAX_INPUT_LEN, file)){
      char *token = strtok(buffer, "\n");
      if (strlen(token) == MAX_INPUT_LEN){
        printError();
        continue;
      } 
      checkMixingCommand(token);
    }
  }
  // extra input file
  else {
    printError();
  }
  return 0;
}
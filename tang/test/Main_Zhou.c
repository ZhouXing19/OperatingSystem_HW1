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
#define READ  0
#define WRITE 1

// STEP 4 Syntax Error *********************************************************
void printError(){
  char error_message[30] = "An ERROR has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
  exit(0);
}

// STEP 3 **********************************************************************
int execRedirectCmd(char* userInput){
    char* cmd = strtok(userInput, ">");
    char* output_fd = strtok(NULL, " ");
    // check invalid input eg. ls > out1 out2
    char* extra = strtok(NULL, " ");
    if(extra != NULL){
      printError();
    }

    int fd1 = open(output_fd, O_RDWR | O_CREAT | O_TRUNC); 
    if (fd1 < 0){
      return 1; 
    }  // TODO: exit(0 or 1)

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


int execSimpleCmd(char* userInput, int inpipe; int first, int last, int input){
  
  // check if dup2 is needed
  //check if this command is in pipe
  if(first != NULL | last != NULL){
      int pipettes[2];
      pipe(pipettes);
        // TO DO: check if we should fork here
    if (first == 1 && last == 0 && input == 0) {
        // First command
        dup2(pipettes[WRITE], STDOUT_FILENO );
    } else if (first == 0 && last == 0 && input != 0) {
        // Middle command
        dup2(input, STDIN_FILENO);
        dup2(pipettes[WRITE], STDOUT_FILENO);
    } else {
        // Last command
        dup2( input, STDIN_FILENO );
    }
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

  if(input != NULL) close(*input);
  if(output != NULL) close(*output);
  if(input != NULL | output!= NULL) return *output;
  return 0;
}

// STEP 2 **********************************************************************
int execNonPipeCmd(char* userInput, int first, int last, int input){
  char* redirectSymbol = strchr(userInput, '>');
  if (redirectSymbol != NULL) execRedirectCmd(userInput);
  else{
    int inpipe = 1;
    execSimpleCmd(userInput, inpipe, first, last, input);
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


  int pid;
  int status;
  int input = 0;
  int first = 1;
  int last = 0;  

// TO DO: Not sure about the exit(0) here.
// Not sure when to fork : execpipecmd or execsimplecmd
  for(int i=0; i<n; i++){
    if((pid = fork())== 0){
      if(i==0){
        input = execNonPipeCmd(parsed[i],1,0,input);
        exit(0);
      }
      if(i != n-1){
        input = execNonPipeCmd(parsed[i],0,0,input);
        exit(0);
      }
      else{
        input = execNonPipeCmd(parsed[i],0,1,input);
        exit(0);
      }
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      if(i == n-1) close(p[0]);
      close(p[1]);
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
      // printf("Return child %d\n", cpid);
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
    // printf("Return child %d\n", cpid);
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
//   char userInput[MAX_INPUT_LEN];
//   int commandType;

//   FILE *file = NULL;
//   char buffer[MAX_INPUT_LEN];

//   // interactive mode
//   if(argc == 1){
//     while(strcmp(userInput,"bye") != 0){
//       printf("520shell>");
//       getUserInput(userInput);
//       checkMixingCommand(userInput);
//     }
//   }

//   // batch mode
//   else if(argc == 2){
//     char *bFile= strdup(argv[1]);
//     if((file = fopen(bFile, "r")) == NULL){
//       printError();
//       return 0;
//     }

//     while(fgets(buffer, MAX_INPUT_LEN, file)){
//       char *token = strtok(buffer, "\n");
//       if (strlen(token) == MAX_INPUT_LEN){
//         printError();
//         continue;
//       } 
//       checkMixingCommand(token);
//     }
//   }
//   // extra input file
//   else {
//     printError();
//   }

  return 0;
}
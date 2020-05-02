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
}

int execSingleCmd(char* userInput, bool fromParallel){
  bool redirect;
  int saved_stdout;

  char redInput[64];
  strcpy(redInput, userInput);
  
  if(strchr(userInput, '>') != NULL){
      redirect = true;
      saved_stdout = dup(1);
      userInput = strtok(userInput, ">");
      char* output_file = strtok(NULL, " ");
      int fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC); 
      if (fd < 0) return 1;
      dup2(fd, STDOUT_FILENO);
    }

    // copy user input for echo parsing
    char copyInput[64];
    int dquote = 0;
    int squote = 0;
    strcpy(copyInput, userInput);
    for(int t=0; t<64; t++){
      if (copyInput[t] == '\"'){
        dquote++;
      }
      else if(copyInput[t] == '\''){
        squote++;
      }
    }

    // general parsing
    char* parsed[20];
    int i=0;
    parsed[i] = strtok(userInput, " ");
    // parsed[i] = strtok(parsed[i], "\t");
    while(parsed[i] != NULL){
      parsed[++i] = strtok(NULL, " ");
      // parsed[i] = strtok(parsed[i], "\t");
    }

    //Regarding parallel mode: all built-in commands should return 
    //an error message in parallel mode.
    if(fromParallel && (strcmp(parsed[0],"bye") == 0 || 
    strcmp(parsed[0],"cd") == 0 || strcmp(parsed[0],"echo") == 0
    || strcmp(parsed[0],"pwd") == 0)){
      if(redirect){
      dup2(saved_stdout, 1); //防止redirect之后print到文件中
      }
      printError();
      return 1;
    }
    
    if(strcmp(parsed[0],"bye") == 0){
      if(parsed[1]==NULL){
        userInput = "bye"; // in seq or par -> terminate parent process
        exit(0); // exit successfully
      }
      else{
        userInput = strcat(parsed[0], "  "); // 防止main while exit
        printError(); // extra arg error
      }
    } 

  
    else if(strcmp(parsed[0],"cd")==0){
      if (strcmp(parsed[0], "") == 0 && parsed[2]!= NULL) {
        if(redirect) dup2(saved_stdout, 1); //防止redirect之后print到文件中
        printError(); 
        return 1;
      } 
      int res = parsed[1] == NULL? chdir(getenv("HOME")): chdir(parsed[1]);
    }
  
    else if(strcmp(parsed[0],"echo")==0){

      // case 1: no args
      if(parsed[1] == NULL){
        write(STDOUT_FILENO, "\n", 1);
      } 
      // case 2: arg starts with '\' 
      else if(strchr(parsed[1], '\\') != NULL){
        printError();
        if(redirect) dup2(saved_stdout, 1); //防止redirect之后print到文件中
        return 1;
      }
      // case 3: arg starts with " 
      else if(parsed[1][0] == '\"'){
        if(dquote == 1){
          printError();
          if(redirect) dup2(saved_stdout, 1); //防止redirect之后print到文件中
          return 1;
        }
        char* cmd = strtok(copyInput, "\"");
        char* str = strtok(NULL, "\""); 
        if(str == NULL){
          write(STDOUT_FILENO, "\n", 1);
        } 
        else{
          str = strcat(str, "\n");
          write(STDOUT_FILENO, str, strlen(str));
        }
      }
      // case 4: arg starts with '
      else if(parsed[1][0] == '\''){
        if(squote == 1){
          printError();
          if(redirect) dup2(saved_stdout, 1); //防止redirect之后print到文件中
          return 1;
        }
        char* cmd = strtok(copyInput, "\'");
        char* str = strtok(NULL, "\'"); 
        if(str == NULL) write(STDOUT_FILENO, "\n", 1);
        else{
          str = strcat(str, "\n");
          write(STDOUT_FILENO, str, strlen(str));
        }
      }
      // case 5: normal case: echo arg
      else{
        char *str = strcat(parsed[1], "\n");
        write(STDOUT_FILENO, str, strlen(str));
        //printf("%s\n", parsed[1]);
      }
    }

    else if(strcmp(parsed[0],"pwd")==0){
      if(parsed[1] != NULL) {
        printError(); // extra arg error
        return 1;
      }
      char cwd[1024]; 
	    getcwd(cwd, sizeof(cwd)); 
      char *str = strcat(cwd, "\n");
      write(STDOUT_FILENO, str, strlen(str));
    }

    // system function:
    else{
      int status;
      pid_t pid = fork();
      if(pid==0){
          if(strcmp(parsed[0], "grep") == 0 && parsed[1] != NULL){
            char* copy1 = parsed[1];
            if(copy1[0] == '\''){
              char* fst = strtok(copy1, "\'");
              parsed[1] = fst;
            }
            else if(copy1[0] == '\"'){
              char* fst = strtok(copy1, "\"");
              parsed[1] = fst;
            }
          }
          execvp(parsed[0], parsed);
      }
      else{
        int cpid = waitpid(pid, &status, 0);
        if(redirect){ //防止redirect之后print到文件中
          dup2(saved_stdout, 1);
        }
      }
    }
    if(redirect) dup2(saved_stdout, 1); //防止redirect之后print到文件中

  return 0;
}

// STEP 3 **********************************************************************
int pipefork(char* userInput, bool inPipe, bool first, bool last, int input, bool fromParallel){
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
    execSingleCmd(userInput, fromParallel);
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

int execPipeCmd(char* userInput, bool fromParallel){
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
      input = pipefork(parsed[i], true, true, false, input, fromParallel);
    }
    else if(i == n-1){
      input = pipefork(parsed[i], true, false, true, input, fromParallel);
    }
    else{
      input = pipefork(parsed[i], true, false, false, input, fromParallel);
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

int parseSingleCommand(char* userInput, bool fromParallel){
  if(strchr(userInput, '>') != NULL && strchr(userInput, '|') != NULL){
    printError();
    return 1;
  }
  else if(strchr(userInput, '|') != NULL){
    execPipeCmd(userInput, fromParallel);
  }
  else{
    execSingleCmd(userInput, fromParallel);
  }
  return 0;
}

int parseSequentialCommand(char* userInput){
  int status;
  pid_t pid;

  char* tokens[20];
  int i=0;
  tokens[i] = strtok(userInput, ";");
  while(tokens[i] != NULL){
    tokens[++i] = strtok(NULL, ";");
  }
  int k = 0;
  // sequential way
  char* token = tokens[k];
  while(token != NULL){
    pid = fork();
    if(pid < 0) return 1;
    
    else if(pid == 0){ // child process
      parseSingleCommand(token, false);
      exit(0);
    }
    else{ // parent process
      int cpid = waitpid(pid, &status, 0);
    }
    // -------- check bye
    char* parsed[20];
    int i=0;
    parsed[i] = strtok(token, " ");
    while(parsed[i] != NULL){
      parsed[++i] = strtok(NULL, " ");
    }
    if(strcmp(parsed[0],"bye") == 0 && parsed[1]==NULL){
      userInput = "bye"; 
      exit(0); // exit successfully
    }
    token = tokens[++k];
  }
  return 0;
}

int parseParallelCommand(char* userInput){
  // printf("--- Parallel Command\n");
  int i, status;
  pid_t pid;
  char* tokens[20];
  int j=0;
  tokens[j] = strtok(userInput, "&");
  while(tokens[j] != NULL){
    tokens[++j] = strtok(NULL, "&");
  }

  int k = 0;
  char* token = tokens[k];

  while(token != NULL){
    pid = fork();
    if(pid < 0) return 1;
    if( pid == 0){
      parseSingleCommand(token, true);
      exit(0);
    }
    token = tokens[++k];
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
  if(sizeof(buf) == 0){
    return 0;
  }
  char *token = strtok(buf, "\n");
  if (sizeof(token) == MAX_INPUT_LEN) {
    printError();
    return 1;
  }
  strcpy(userInput, buf);

  return 0;
}

int checkMixingCommand(char* userInput){
    const char and = '&';
    const char semi = ';';
    char *findAnd = strchr(userInput, and);
    char *findSemi = strchr(userInput, semi);
    if(findAnd == NULL && findSemi == NULL){
      parseSingleCommand(userInput, false);
    } 
    else if(findAnd == NULL && findSemi != NULL){
      parseSequentialCommand(userInput);
    }
    else if(findAnd != NULL && findSemi == NULL){
      parseParallelCommand(userInput);
    }
    else{
      printError();
      return 1;
    }

  return 0;
}

int main(int argc, char** argv){
  char userInput[MAX_INPUT_LEN];
  int commandType;

  FILE *file;
  char buffer[MAX_INPUT_LEN];

  // interactive mode
  if(argc == 1){
    while(strcmp(userInput,"bye") != 0){
      write(STDOUT_FILENO, "520shell> ", 10);
      getUserInput(userInput);
      checkMixingCommand(userInput);
    }
  }

  // batch mode
  else if(argc == 2){
    char *bFile= strdup(argv[1]);
    file = fopen(bFile, "r");
    if(file == NULL){
      printError();
      return 1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    char* token = "";
    while((read = getline(&line, &len, file) != -1) && strcmp(token,"bye") != 0){
      write(STDOUT_FILENO, line, strlen(line));
      token = strtok(line, "\n");
      if(token == NULL){
        return 0;
      }
      if (strlen(token) >= MAX_INPUT_LEN){
        // return 1;
        continue;
      }
      else{
        checkMixingCommand(token);
      }
    }
    fclose(file);
  }
  // extra input file
  else {
    printError();
    return 1;
  }
  return 0;
}
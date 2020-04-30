#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 
#include<ctype.h>

int BSIZE= 512;
char ERROR_MESSAGE[128] = "An error has occurred\n";
static char *args[256];

void printError(){
        write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE));
        exit(1);
        
}


void printPrompt(){
        write(STDOUT_FILENO, "wish> ", strlen("wish> "));                
}


void split(char* cmd){
  // int i = 0 ; 
  // char *pch;
  // pch = strtok (cmd," ");
  // args[i] = pch;
  // while (pch != NULL)
  // {
  //   i++;
  //   pch = strtok (NULL, " ");
  //   args[i] = pch;
  // }
  // args[i+1] = NULL;
  int i=0;
  args[i] = strtok(cmd, " ");
  while(args[i] != NULL){
    args[++i] = strtok(NULL, " ");
  }
}

int executeCmd(char* userInput){
  printf("Testing");
  return 0;
}


int main(int argc, char** argv){
  FILE *file = NULL;
  char buffer[BSIZE];
  char line[BSIZE];
        
  int batch = 0;
        
  if(argc==2){ //Batch mode
    char *bFile= strdup(argv[1]);
    file = fopen(bFile, "r");
    if (file == NULL) {
      printError();
    }
    batch = 1;
  }
  else{
    printError(); // more than 1 file
  }

  while(fgets(buffer, BSIZE, file)){
    printf("Buffer is %s,\n", buffer); // buffer last char
    //print out the command
    int status;
    pid_t pid = fork();    
    if(pid < 0) return -1;
    else if(pid == 0){
      executeCmd(buffer);
      // printf("In child process, child_pid is %d\n", getpid());
      exit(0);
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      printf("Return child1 %d\n", cpid);
    }
  }
        return 0;
}
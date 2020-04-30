#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 

#define MAX_INPUT_LEN 64

int getUserInput(char* userInput){
  char buf[MAX_INPUT_LEN]; 
  fgets(buf, MAX_INPUT_LEN, stdin);
  char *token = strtok(buf, "\n");
  strcpy(userInput, buf);
  return 0;
}

int main(){ 
  int inputFd = open("myFile", O_RDWR);
  char* parsed1[20];
  parsed1[0] = "ls";
  parsed1[1] = "-la";

  char* parsed2[20];
  parsed2[0] = "echo";
  printf("1HERE!");
  char inbuf[1024];
  int p[2], status, nbytes;
  pid_t pid;
  printf("HERE!");
  if(pipe(p)<0) exit(1);
  dup2(p[0], STDIN_FILENO); // read end of the pipe
  // dup2(p[1], STDOUT_FILENO); // write end of the pipe
  
  // test p[0] == null
  nbytes = read(p[0], inbuf, 1024);
  printf("nbytes is %s\n", inbuf);
  return 0;

  if((pid = fork())== 0){
      dup2(p[1], STDOUT_FILENO);
      int res = execvp(parsed1[0], parsed1);
      exit(0);
    }
  else{
    int cpid = waitpid(pid, &status, 0);
  }

  if((pid = fork())== 0){
      nbytes = read(p[0], inbuf, 1024);
      dup2(STDOUT_FILENO, STDOUT_FILENO);
      printf("get the output %s\n", inbuf);
      exit(0);
    }
  else{
    int cpid = waitpid(pid, &status, 0);
    
  }
    return 0; 
}
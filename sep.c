#include <limits.h>
#include <stdio.h>
#include <stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 
#include<sys/wait.h>

void printError(){
  char error_message[30] = "An ERROR has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
  exit(0);
  // TODO: exit gracefully: process or program, where to put exit(0)
}

int testSingleCommand(int count, int *p){
  dup2(*p, STDOUT_FILENO);
  printf("Executing ----------------\n");
  sleep(5-count); 
  printf("process %d is done\n", count);
  return 0;
}

int main(){
  int i, status;
  pid_t pid;

  char inbuf[64]; // used to get the input
  int p[2];
  int readStatus;

  pipe(p);
  // write(p[1], "", 1024);
  // sequential way
  for(i=0; i<5; i++){
    if((pid = fork())== 0){
      // printf("inbuf in child closure is %s\n", inbuf);
      dup2(p[1], STDOUT_FILENO);
      testSingleCommand(i, &p[1]);
      exit(0);
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      readStatus = read(p[0], inbuf, 1024);
      printf("inbuf in parent closure is \n %s\n", inbuf);
      // printf("Return child %d\n", cpid);
    }
  }

  // -------- PARALLEL ------------
  // for(i=0; i<5; i++){
  //   if((pid = fork())== 0){
  //     testSingleCommand(i);
  //     exit(0);
  //   }
  // }
  
  // while (wait(NULL) != -1){
  //   pid_t cpid = waitpid(pid, &status, 0);
  //   printf("Return child %d\n", cpid);
  // }

  // pid_t cpid;
  // while((cpid = wait(& status)) != 0){
  //   printf("Get returned child process %d\n", cpid);
  // }


  return 0;
}
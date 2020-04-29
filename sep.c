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

int testSingleCommand(int count){
  if(count == 2){
    printError();
  }
  printf("Executing ----------------\n");
  sleep(5-count); 
  printf("Count = %d, In child process, child_pid is %d\n", count, getpid());
  return 0;
}

int main(){
  int i, status;
  pid_t pid;
  // sequential way
  for(i=0; i<5; i++){
    if((pid = fork())== 0){
      testSingleCommand(i);
      exit(0);
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      printf("Return child %d\n", cpid);
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
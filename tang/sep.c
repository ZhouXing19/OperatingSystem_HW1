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
  // if(count == 2){
  //   printError();
  // }
  int status1;
  pid_t pid1;
  if((pid1 = fork())== 0){
  printf("Executing ----------------\n");
  sleep(5-count); 
  printf("Count = %d, In child process, child_pid is %d\n", count, getpid());
  }
  else{
      int cpid1 = waitpid(pid1, &status1, 0);
      printf("Return child1 %d\n", cpid1);
  }


  return 0;
}

int main(){
  int i, status;
  pid_t pid;
  i=0;


   // -------- SEQUENTIAL ------------
  while(i<5){
    
    if((pid = fork())== 0){
      printf("flag \n");
      testSingleCommand(i);
      exit(0);
    }
    else{
      int cpid = waitpid(pid, &status, 0);
      printf("Return child %d\n", cpid);
    }
    i++;
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
  //   printf("Parent: %d, Return child %d\n", pid, cpid);
  // }

  // pid_t cpid;
  // while((cpid = wait(& status)) != 0){
  //   printf("Get returned child process %d\n", cpid);
  // }


  return 0;
}
#include <limits.h>
#include <stdio.h>
#include <stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 
#include<sys/wait.h>

int testSingleCommand(int count){
  printf("Executing ----------------\n");
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
  //   if((pid[i] = fork())== 0){
  //     testSingleCommand(i);
  //     exit(0);
  //   }
  // }
  
  // while (wait(NULL) != -1){
  //   pid_t cpid = waitpid(pid[i], &status, 0);
  //   printf("Return child %d\n", cpid);
  // }

  // pid_t cpid;
  // while((cpid = wait(& status)) != 0){
  //   printf("Get returned child process %d\n", cpid);
  // }


  return 0;
}
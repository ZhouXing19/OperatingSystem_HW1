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
int exec(){
  printf("Where is my miu? \n");
  int fd = open("x2.txt", O_RDWR | O_CREAT | O_TRUNC);
  if(fd>0){
    dup2(fd, STDOUT_FILENO);
    printf("Here is my miu\n");
  }
  return 0;
}

int main(){
  int fd = open("x1.txt", O_RDWR | O_CREAT | O_TRUNC);
  printf("before dup2");
  if(fd>0){
    dup2(fd, STDOUT_FILENO);
    exec();
  }
  printf("Out of scope");
}
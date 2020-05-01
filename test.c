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
  for(int i = 0; i< 5; i++){
    if(i == 3) exit(0);
    printf("i is id %d\n", i);
  }
  return 0;
}

int main(){
  exec();
  printf("still her e\n");
}
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

int main(){
  char inbuf[1024];
  int p[2], nbytes;
  pipe(p);
  write(p[1], "", 1); 
  // nbytes = read(p[0], inbuf, 1024);
  printf("nbytes is %s\n", inbuf);
  return 0;
}
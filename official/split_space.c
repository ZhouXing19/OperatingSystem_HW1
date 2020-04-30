
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/wait.h>

static char* args[512];


void split(char* cmd){
  int i = 0 ; 
  char *pch;
  pch = strtok (cmd," ,.-");
  args[i] = pch;
  while (pch != NULL)
  {
    i++;
    pch = strtok (NULL, " ,.-");
    args[i] = pch;
  }
}




int main ()
{
  char str[] ="echo";
  split(str);  
  return 0;
  
}
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
char* output_file = strtok(NULL, " ");
      int fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC); 
      if (fd < 0) return 1;
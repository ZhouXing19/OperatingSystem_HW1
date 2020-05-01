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

int main(){
    char *str = "hello world";
    write(STDOUT_FILENO, str, strlen(str));
    return 0;
}
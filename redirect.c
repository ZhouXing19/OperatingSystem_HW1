#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

extern int errno;
int main(){

  int fd1 = open("miu", O_RDWR); 
    if (fd1 < 0)  
    { 
        perror("c1"); 
        exit(1); 
    } 
    printf("opened the fd = % d\n", fd1); 

    
    dup2(fd1, STDOUT_FILENO);

    //TODO: add ls exec to test!
    char* parsed[20];
    parsed[0] = "ls";
    parsed[1] = "-la";
    parsed[2] = NULL;
    parsed[3] = NULL;

    if (execvp(parsed[0], parsed) < 0) { 
      printf("\nCould not execute command.."); 
    } 


    // Using close system Call 
    if (close(fd1) < 0)  
    { 
        perror("c1"); 
        exit(1); 
    }  
    
}
#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 

# define MAX_INPUT_LEN 64

char** cmdToArray(char* userInput){
  int p=0;
  char* parsed[]= {"hello"};
  char* token = strtok(userInput, " ");
  while(token != NULL){
    parsed[p] = token;
    token = strtok(NULL, " ");
    p++;
  }
  return parsed;
}

int main(){
  char* input = "this  is my miumiu.";
  char** parsed = cmdToArray(input);
  // for(int i=0; i<sizeof(parsed); i++){
  //   printf("%s\n", parsed[i]);
  // }
  // printf("%s\n", parsed[0]);
}
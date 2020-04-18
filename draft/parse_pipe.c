
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define MAXCOM 64 // max number of bytes to be supported 
#define MAXLIST 100 // max number of commands to be supported 


int parsePipe(char* str, char** pipe_seped, char* two_D_pipe_seped[MAXLIST][MAXLIST]){
	int i = 0;
    int j = 0;
    int k = 0;

    if(str!=NULL){
        pipe_seped[i] = strtok(str,"|");
        while(pipe_seped[i]!=NULL){
            i+=1;
            pipe_seped[i] = strtok(NULL,"|");
        }
    }

    //printf("stage 1 \n");

    while(pipe_seped[j]!=NULL){
        char demostring[MAXLIST];
        strcpy(demostring,pipe_seped[j]);
        printf("\n demostring: %s \n",  demostring);
        two_D_pipe_seped[j][k] = strtok(demostring," ");
        printf("\n ***  %d :", j);
        while(two_D_pipe_seped[j][k]!=NULL){
            printf("%s ",  two_D_pipe_seped[j][k]);
            k++;
            two_D_pipe_seped[j][k] = strtok(NULL," ");
        }
        j++;
    }

    for(int t=0;t<i;t++){
        printf("\n %d of | : %s \n", t, pipe_seped[t]);
    }
    //printf(" \n stage 2");
	return i; //return after seperating input with &, we get how many command
}


int main() 
{ 
	char inputString[MAXCOM], *parsedArgs[MAXLIST]; 

	char *pipe_sep[MAXLIST], *semi_sep[MAXLIST], *and_sep[MAXLIST];

	char *multi_semi_col[MAXLIST][MAXLIST];
	char *multi_and[MAXLIST][MAXLIST];
	char *multi_pipe[MAXLIST][MAXLIST];


    scanf("%[^\n]%*c", inputString);
    int pipecnt = 0;

    pipecnt = parsePipe(inputString,pipe_sep,multi_pipe);

    for(int i=0;i<pipecnt;i++){
        printf("\n ***  %d :", i);
        int j =0;
        while(multi_pipe[i][j]!=NULL){
            printf(" ^ %s ",  multi_pipe[i][j]);
            j++;
        }
    } 
    return 0;

} 

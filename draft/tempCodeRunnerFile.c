
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


void parseSpace(char* str, char** parsed) 
{   
	int i; 


	for (i = 0; i < MAXLIST; i++) { 
        
		parsed[i] = strsep(&str, " "); 

		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i--; 
	}
} 

void parsePipe(char* str, char** pipe_seped){
	int i = 0;


    if(str!=NULL){
        pipe_seped[i] = strtok(str,"|");
        while(pipe_seped[i]!=NULL){
            i+=1;
            pipe_seped[i] = strtok(NULL,"|");
        }
    }
    //return after seperating input with &, we get how many command
}


int main() 
{ 
	char inputString[MAXCOM], *parsedArgs[MAXLIST]; 

	char *pipe_sep[MAXLIST], *semi_sep[MAXLIST], *and_sep[MAXLIST];

    char demostring[MAXLIST];
    char* to_execute[MAXLIST];

    scanf("%[^\n]%*c", inputString);
    
    parsePipe(inputString,pipe_sep); 
    
    strcpy(demostring,pipe_sep[0]);


    printf("%s \n",demostring);

    parseSpace(demostring,to_execute);
    printf("aaa %s \n",to_execute[0]);
    printf("aaa %s \n",to_execute[1]);
    printf("aaa %s \n",to_execute[2]);


    // char *demolist[MAXLIST];
    // printf("aaa %s \n",demolist[0]);
    // demolist[0] = "12345";
    // printf("bbb %s \n",demolist[0]);

    return 0;

} 

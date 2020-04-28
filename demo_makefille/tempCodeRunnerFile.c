#include <stdio.h>
#include<string.h>

#define MAXCOM 64 // max number of bytes to be supported 
#define MAXLIST 100 // max number of commands to be supported 

int parsePipe(char* str, char** strpiped) 
{ 
	int i; 
    if(str!=NULL){
        strpiped[i] = strtok(str,"|");
        while(strpiped[i]!=NULL){
            i+=1;
            strpiped[i] = strtok(NULL,"|");
        }
    }
	return i;
} 


// int parseSpace(char* str, char* strpiped[256]) 
// { 
// 	int i; 
//     if(str!=NULL){
//         strpiped[i] = strtok(str," ");
//         while(strpiped[i]!=NULL){
//             i+=1;
//             strpiped[i] = strtok(NULL," ");
//         }
//     }
// 	return i;
// } 





int main(){
    char *string[256];
    char *str2[256][256];
    char input[1064];

    int i = 0;


    scanf("%[^\n]%*c", input); 
    i = parsePipe(input, string);
    printf("%d i_after",i);
    
    
    return 0;


}
#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<unistd.h> 
#include<readline/readline.h> 
#include<ctype.h>

int BSIZE= 512;
char ERROR_MESSAGE[128] = "An error has occurred\n";
char *path;
static char *args[256];

// int fork1(void) {
// 	int pid;

// 	pid = fork();
// 	if (pid == -1)
// 		perror("fork");
// 	return pid;
// }

void printError(){
        write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE));
        exit(1);
        
}


void printPrompt(){
        write(STDOUT_FILENO, "wish> ", strlen("wish> "));                
}


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
  args[i+1] = NULL;
}

void execute(int i, char* buffer){
    int status;
    pid_t pid;
    if((pid = fork())== 0){
         write(STDOUT_FILENO, buffer, strlen(buffer));

            //execute command
            
            // for(int k=0;k<4;k++){
            //     printf("args[%d] =%s ;",k,args[k]);
            // }

           sleep(20-i); 
           execvp(args[0],args);
           
           //exit(0);
           printf("In child process, child_pid is %d\n", getpid());
           }
            
    else{
        int cpid1 = waitpid(pid, &status, 0);
        printf("Return child1 %d\n", cpid1);
        }

}



int main(int argc, char** argv){
        FILE *file = NULL;
        //path=(char*) malloc(BSIZE);
        char buffer[BSIZE];
        char line[BSIZE];
        
        int batch = 0;
        
        if(argc==1){ //Not batch mode
                file=stdin;
                printPrompt();
                 //Store standard input to the file.
        }

        else if(argc==2){ //Batch mode
                
                char *bFile= strdup(argv[1]);
                file = fopen(bFile, "r");
                if (file == NULL) {
        	        printError();
                }
                batch = 1;
        }
        else{
                printError();
        }

        int i = 0;
        while(fgets(buffer, BSIZE, file)){
            //print out the command
            int status;
            pid_t pid;    


            printf("Executing command [%d]----------------\n",i);

            // print command
            strcpy(line,buffer); 
            split(line);

            execute(i,buffer);

            i++;
            if(batch==0)
            printPrompt();
         }
        return 0;

        // 
        // int numTomatoes = 10;
        // size_t length = snprintf(buffer, sizeof(buffer), "I would like %d tomatoes", numTomatoes);
        // write(STDOUT_FILENO, buffer, length);


}
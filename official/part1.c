// C Program to design a shell in Linux 
// How to run this c file : 
// 1. Open Terminal
// 2. gcc -g -lreadline part1_4.c -o mysh
// 3. /mysh


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

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

// Greeting shell during startup 
void init_shell() 
{ 
	clear(); 
} 

// Function to take input 
int takeInput(char* str) 
{ 
	char* type_in; 
	printf("\n");
	type_in = readline("520shell>"); 
	if (strlen(type_in) != 0) { 
		strcpy(str, type_in); 
		return 0; 
	} else { 
		return 1; 
	} 
} 

// Function to print Current Directory. 
void printDir() 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	printf("\nDir: %s", cwd); 
} 



// Function where the system command is executed 
void execArgs(char** parsed, int* flag,int cnt) 
{ 
    
	// Forking a child 
	pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} else if (pid == 0) { 
		//execvp: 传入一个char** 的 list of string, 第一个argument是list[0],第二个argument是list本身。
		//e.g. parsed = ['git','push','origin','master']
		if (execvp(parsed[0], parsed) < 0) {
        if(cnt>0){
            printf("\nCould not execute command..");
            exit(0); 
        }
        (*flag) = -1;
        //printf("flag in execArgs = %d, cnt = %d",*flag,cnt);
        return ;
        }
		
	} else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
} 


// Function to execute builtin commands 
int buildin(char** parsed) 
{ 
    if(strcmp(parsed[0],"bye")==0){
        if(parsed[1]==NULL){
        printf("Goodbye. \n");
        exit(0);
        }
        else{
            printf("Too many arguments for bye");
            return 1;
        }

    }
    else if(strcmp(parsed[0],"cd")==0){
        if(parsed[1]==NULL){
            chdir(getenv("HOME"));
        }
        else{
            chdir(parsed[1]);
        }
        return 1;
    }
    else if(strcmp(parsed[0],"echo")==0){
        int t=1;
        if(parsed[1]==NULL){
            return 1;
        } 
        while(parsed[t]!=NULL){
            if(t>1){
                printf(" ");
            }
			printf("%s",parsed[t]);
			t++;
		}
        return 1;
    }
    else if(strcmp(parsed[0],"pwd")==0){
        if(parsed[1]==NULL){
        printDir();
        }
        else{
            printf("Too many arguments for pwd");
        }
        return 1;


    }
	return 0; 
} 

void run_command(char** parsed, int* flag, int cnt){
    *flag = buildin(parsed);
    if(*flag==0)
        execArgs(parsed,flag,cnt); 
   // printf("\n flag in run_command = %d, cnt = %d",*flag,cnt);
    return;
}

//把命令按空格划分，每个command依次存入strpiped
//返回有多少个被空格划分的command
// function for parsing command words 
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


//function for finding multiple commands
//如果输入的指令存在分号，先按照分号，把指令分割成n份，将n个command传入semi_seped.
//然后对每个command，按照空格划分为m个string，依次存入two_D_semi_seped
//在two_D_semi_seped这个矩阵中，第n行，第m列 = 第n个command的第m个字符串
int parseSemiCol(char* str, char** semi_seped){
	int i = 0;
    int flag = 0;


    if(str!=NULL){
        
        semi_seped[i] = strtok(str,";");

        while(semi_seped[i]!=NULL){
            char* to_execute[MAXLIST];
            char demostring[MAXLIST];
            strcpy(demostring,semi_seped[i]);

            parseSpace(demostring,to_execute);
            run_command(to_execute,&flag,i);
           // printf("\n flag in parseSemiCol = %d",flag);
            if(flag==-1){
                printf("Cannot run with parsed.");
                return 0;
            }
            i+=1;
            semi_seped[i] = strtok(NULL,";");
        }
        return 1;
    }

    
	//返回有多少个被分号分割的指令。
	return i; //return after seperating input with ;, we get how many command
}

//function for finding multiple commands
//如果输入的指令存在&，先按照&，把指令分割成n份，将n个command传入and_seped.
//然后对每个command，按照空格划分为m个string，依次存入two_D_and_seped
//在two_D_and_seped这个矩阵中，第n行，第m列 = 第n个command的第m个字符串
int parseAnd(char* str, char** and_seped, char* two_D_and_seped[MAXLIST][MAXLIST]){
	int i = 0;
    int j = 0;
    int k = 0;

    if(str!=NULL){
        and_seped[i] = strtok(str,"&");
        while(and_seped[i]!=NULL){
            i+=1;
            and_seped[i] = strtok(NULL,"&");
        }
    }

    while(and_seped[j]!=NULL){
        two_D_and_seped[j][k] = strtok(and_seped[j]," ");
        while(two_D_and_seped[j][k]!=NULL){
            k++;
            two_D_and_seped[j][k] = strtok(NULL," ");
        }
        j++;
    }
	return i; //return after seperating input with &, we get how many command
}



//function for finding multiple commands
//如果输入的指令存在|，先按照|，把指令分割成n份，将n个command传入pipe_seped.
//然后对每个command，按｜划分为m个string，依次存入two_D_pipe_seped
//在two_D_pipe_seped这个矩阵中，第n行，第m列 = 第n个command的第m个字符串
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

    while(pipe_seped[j]!=NULL){
        two_D_pipe_seped[j][k] = strtok(pipe_seped[j]," ");
        while(two_D_pipe_seped[j][k]!=NULL){
            k++;
            two_D_pipe_seped[j][k] = strtok(NULL," ");
        }
        j++;
    }
	return i; //return after seperating input with &, we get how many command
}




int processString(char* str, char** pipe_sep, char* multi_pipe[MAXLIST][MAXLIST], \
				char** semi_sep, char* multi_semi_col[MAXLIST][MAXLIST], \
				char** and_sep, char* multi_and[MAXLIST][MAXLIST],
				char** parsedArgs) 
{ 
	int pipecnt = 0; 
	int semicnt = 0;
	int andcnt = 0;
	int spacecnt = 0;

	pipecnt = parsePipe(str, pipe_sep, multi_pipe); 
	semicnt = parseSemiCol(str,semi_sep);
	andcnt = parseAnd(str,and_sep,multi_and);

	//这里只写了一种情况：传入的命令没有以上三种符号，则直接按照空格划分
	//保存在parsedArgs中，执行。
	
	if(pipecnt==0 && semicnt==0 && andcnt==0){
		parseSpace(str,parsedArgs);
	}

	//如果是预设命令，直接执行
	if (buildin(parsedArgs)) 
		return 0; 
	//否则，扔个main继续处理
	else
		return 1; 
} 

int main() 
{ 
	char inputString[MAXCOM], *parsedArgs[MAXLIST]; 

	char *pipe_sep[MAXLIST], *semi_sep[MAXLIST], *and_sep[MAXLIST];

	char *multi_semi_col[MAXLIST][MAXLIST];
	char *multi_and[MAXLIST][MAXLIST];
	char *multi_pipe[MAXLIST][MAXLIST];


	int execFlag = 0;



	init_shell(); 

	while (1) { 
		// take input 
		if (takeInput(inputString)) 
			continue; 
        

        execFlag = parseSemiCol(inputString,semi_sep);
        
        if(execFlag==0){
            parseSpace(inputString,parsedArgs);
            run_command(parsedArgs,0,0);
        }
	} 	 

return 0;
}

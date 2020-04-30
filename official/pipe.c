/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/wait.h>
 
/* The array below will hold the arguments: args[0] is the command. */
static char* args[512];
pid_t pid;
int command_pipe[2];
 
#define READ  0
#define WRITE 1
 
static char line[1024];
static int n = 0; /* number of calls to 'command' */


////// STEP 3: 根据当前command的位置（开头，中间，结尾）来执行command
static int command(int input, int first, int last)
{
	int pipettes[2];
 
	/* Invoke pipe */
	pipe( pipettes );	
	pid = fork();
 

	if (pid == 0) {
		if (first == 1 && last == 0 && input == 0) {
			// First command
			dup2( pipettes[WRITE], STDOUT_FILENO );
		} else if (first == 0 && last == 0 && input != 0) {
			// Middle command
			dup2(input, STDIN_FILENO);
			dup2(pipettes[WRITE], STDOUT_FILENO);
		} else {
			// Last command
			dup2( input, STDIN_FILENO );
		}


//////////////////////////////////////
////// 有问题的部分 ⬇️
//////////////////////////////////////
//////////////////////////////////////

		if (strcmp(args[0],"hello")==0)
		{
			write(STDOUT_FILENO, "hello zhou", strlen("hello zhou"));
		}

		////////////// 上面是buildin 
		else{
			int res = execvp( args[0], args);
			printf("after execvp"); //这行是不会执行的
			if(res == -1)
				_exit(EXIT_FAILURE); 
				// If child fails
		}
			
	}
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
 
	if (input != 0) 
		close(input);

	// Nothing more needs to be written
	close(pipettes[WRITE]);
 
	// If it's the last command, nothing more needs to be read
	if (last == 1)
		close(pipettes[READ]);


	return pipettes[READ];
}
 
/* Final cleanup, 'wait' for processes to terminate.
 *  n : Number of times 'command' was invoked.
 */
static void cleanup(int n)
{
	int i;
	for (i = 0; i < n; ++i) 
		wait(NULL); 
}
 

static void split(char* cmd);

////// STEP 2: 把当前command按空格切分且存放到args中(list array) 
 
static int run(char* cmd, int input, int first, int last)
{	
	// 按空格切分
	split(cmd);
	//printf("we can split");
	if (args[0] != NULL) {
		if (strcmp(args[0], "exit") == 0) 
			exit(0);
		n += 1;
		return command(input, first, last);
	}
	return 0;
}
 
static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}
 
static void split(char* cmd)
{	
	
	cmd = skipwhite(cmd);
	
	char* next = strchr(cmd, ' ');
	//printf("1 \n");
	int i = 0;
 
	while(next != NULL) {
		
		next[0] = '\0';
		args[i] = cmd;
		++i;
		
		cmd = skipwhite(next + 1);
		next = strchr(cmd, ' ');
	}

	if (cmd[0] != '\0') {
		args[i] = cmd;
		//printf("3 \n");
		next = strchr(cmd, '\n');
		//cmd = "ls"
		if(next == NULL){
			args[i+1] = NULL;
			return ;
		}
		next[0] = '\0';
		++i; 
	}
 
	args[i] = NULL;
}



////// STEP 1: 按｜切割最初的command，切下来的部分存为 cmd ， 剩下的部分存为 next
////////////// while loop 顺序切割， 切一段，执行一段。只需要标记切下来这部分是 头 中间 尾巴。
int main()
{
	printf("SIMPLE SHELL: Type 'exit' or send EOF to exit.\n");
	while (1) {
		/* Print the command prompt */
		printf("$> ");
		fflush(NULL);
 
		/* Read a command line */
		if (!fgets(line, 1024, stdin)) 
			return 0;
 
		int input = 0;
		int first = 1;
 
		char* cmd = line;
		char* next = strchr(cmd, '|'); /* Find first '|' */
		//printf("cmd:%s, next:%s \n",cmd,next);
 
		while (next != NULL) {
			/* 'next' points to '|' */
			//printf("run: cmd:%s \n",cmd);
			*next = '\0';
			input = run(cmd, input, first, 0);
			//printf("after run: cmd:%s, next:%s \n",cmd,next);
 
			cmd = next + 1;
			
			next = strchr(cmd, '|'); /* Find next '|' */
			first = 0;
		}

		//printf("last run: cmd:%s \n",cmd);
		input = run(cmd, input, first, 1);
		cleanup(n);
		n = 0;
	}
	return 0;
}
 
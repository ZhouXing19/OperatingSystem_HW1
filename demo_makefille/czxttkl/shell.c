/*
 ============================================================================
 Name        : sh.c
 Author      : Zhengxing Chen
 Version     :
 Copyright   : czxttkl. Achieve Shell basic functions: execution, IO redirection and Pipe.
 ============================================================================
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Simplifed xv6 shell.

#define MAXARGS 10

// All commands have at least a type. Have looked at the type, the code
// typically casts the *cmd to some specific cmd type.
struct cmd {
	int type; //  ' ' (exec), | (pipe), '<' or '>' for redirection
};

struct execcmd {
	int type; // ' '
	char *argv[MAXARGS]; // arguments to the command to be exec-ed
};

struct redircmd {
	int type; // < or >
	struct cmd *cmd; // the command to be run (e.g., an execcmd)
	char *file; // the input/output file
	int mode; // the mode to open the file with
	int fd; // the file descriptor number to use for the file
};

struct pipecmd {
	int type; // |
	struct cmd *left; // left side of pipe
	struct cmd *right; // right side of pipe
};

int fork1(void); // Fork but exits on failure.
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void runcmd(struct cmd *cmd) {
	int p[2], r;
	struct execcmd *ecmd;
	struct pipecmd *pcmd;
	struct redircmd *rcmd;

	if (cmd == 0)
		exit(0);

	switch (cmd->type) {
	default:
		fprintf(stderr, "unknown runcmd\n");
		exit(-1);

	case ' ':
		ecmd = (struct execcmd*) cmd;
		if (ecmd->argv[0] == 0)
			exit(0);
		if (strcmp(ecmd->argv[0],"hello")==0)
		{
			printf("hello zhou!");
			break;
		}
		
		execvp(ecmd->argv[0], ecmd->argv);

		break;

	case '>':
	case '<':
		rcmd = (struct redircmd*) cmd;

		// Redirected file descriptor
		int rfd;

		if ((rfd = open(rcmd->file, rcmd->mode, 0777)) != -1) {
			dup2(rfd, rcmd->fd);
			close(rfd);
		} else {
			fprintf(stderr, "Can't redirect to the file");
		}

		runcmd(rcmd->cmd);
		break;

	case '|':
		pcmd = (struct pipecmd*) cmd;

		//Bytes written on pipefd[1] can be read from pipefd[0].
		int pipefd[2];

		if (pipe(pipefd) == -1) {
			perror("pipe error");
			exit(EXIT_FAILURE);
		}

		if (!fork1()) {
			// Child process runs the left command
			// Map stdout to pipefd[1]
			close(STDOUT_FILENO);
			dup(pipefd[1]);
			runcmd(pcmd->left);
		} else {
			// Parent process runs the right command
			// Map stdin to pipefd[0]
			close(STDIN_FILENO);
			dup(pipefd[0]);
			close(pipefd[1]);
			runcmd(pcmd->right);
		}

		break;
	}
	exit(0);
}

int getcmd(char *buf, int nbuf) {

	if (isatty(fileno(stdin)))
		fprintf(stdout, "$ ");
	memset(buf, 0, nbuf);
	fgets(buf, nbuf, stdin);
	if (buf[0] == 0) // EOF
		return -1;
	return 0;
}



int fork1(void) {
	int pid;

	pid = fork();
	if (pid == -1)
		perror("fork");
	return pid;
}

struct cmd*
execcmd(void) {
	struct execcmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = ' ';
	return (struct cmd*) cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type) {
	struct redircmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = type;
	cmd->cmd = subcmd;
	cmd->file = file;
	cmd->mode = (type == '<') ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
	cmd->fd = (type == '<') ? 0 : 1;
	return (struct cmd*) cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right) {

	//printf("left:%s, right:%s \n",left->ar,right->argv[0]);
	struct pipecmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = '|';
	cmd->left = left;
	cmd->right = right;
	return (struct cmd*) cmd;
}

// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int gettoken(char **ps, char *es, char **q, char **eq) {
	char *s;
	int ret;

	s = *ps;
	while (s < es && strchr(whitespace, *s))
		s++;
	if (q)
		*q = s;
	// q：第一个不再whitespace里的字符的地址
	// ret：这个字符
	ret = *s;
	switch (*s) {
	case 0:
		break;
	case '|':
	case '<':
		s++;
		break;
	case '>':
		s++;
		break;
	default:
		ret = 'a';
		while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
			s++;
		break;
	}
	if (eq)
		*eq = s;

	while (s < es && strchr(whitespace, *s))
		s++;
	*ps = s;
	return ret;
}

int peek(char **ps, char *es, char *toks) {
	char *s;

	//s是string ps的头指针 （指向第一个char的指针）
	//ps是指向string的头的指针（也是指向第一个char的指针吧）
	s = *ps;
	//遍历ps的每个字符是不是出现在 char whitespace[] = " \t\r\n\v"; 中
	while (s < es && strchr(whitespace, *s))
		s++;
	//把 指向string头的指针改成s --> string的头变成了s 
	//（string只保留s及后面的部分）
	*ps = s;

	//判断这个新的开头字符是不是非空且出现在toks里
	return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

// make a copy of the characters in the input buffer, starting from s through es.
// null-terminate the copy to make it a string.
char *mkcopy(char *s, char *es) {
	int n = es - s;
	char *c = malloc(n + 1);
	assert(c);
	strncpy(c, s, n);
	c[n] = 0;
	return c;
}

struct cmd*
parsecmd(char *s) {
	char *es;
	struct cmd *cmd;

	es = s + strlen(s);
	cmd = parseline(&s, es);
	peek(&s, es, "");
	if (s != es) {
		fprintf(stderr, "leftovers: %s\n", s);
		exit(-1);
	}
	return cmd;
}

struct cmd*
parseline(char **ps, char *es) {
	struct cmd *cmd;
	cmd = parsepipe(ps, es);
	return cmd;
}

struct cmd*
parsepipe(char **ps, char *es) {
	//printf(" before parse pipe: \n ====== \n ps:%s, es:%s \n +++++++ \n",*ps,es);
	struct cmd *cmd;

	cmd = parseexec(ps, es);
	if (peek(ps, es, "|")) {
		gettoken(ps, es, 0, 0);
		cmd = pipecmd(cmd, parsepipe(ps, es));
	}
	//printf(" after parse pipe: \n ====== \n ps:%s, es:%s \n +++++++ \n",*ps,es);

	//printf("cmd->type: %c",(char)cmd->type);
	return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es) {
	int tok;
	char *q, *eq;

	while (peek(ps, es, "<>")) {
		tok = gettoken(ps, es, 0, 0);
		if (gettoken(ps, es, &q, &eq) != 'a') {
			fprintf(stderr, "missing file for redirection\n");
			exit(-1);
		}
		switch (tok) {
		case '<':
			cmd = redircmd(cmd, mkcopy(q, eq), '<');
			break;
		case '>':
			cmd = redircmd(cmd, mkcopy(q, eq), '>');
			break;
		}
	}
	return cmd;
}

struct cmd*
parseexec(char **ps, char *es) {
	// ps: "ls | grep .c " / ps: "grep.c"
	//printf("*ps=%s, es=%s ",*ps,es);
	char *q, *eq;
	int tok, argc;
	struct execcmd *cmd;
	struct cmd *ret;

	ret = execcmd();
	cmd = (struct execcmd*) ret;

	argc = 0;
	ret = parseredirs(ret, ps, es);
//	printf("argv:");
	while (!peek(ps, es, "|")) {
			printf("\n *ps=%s, es=%s, q=%s, eq=%s \n",*ps,es,q,eq);
		if ((tok = gettoken(ps, es, &q, &eq)) == 0)
			break;
		if (tok != 'a') {
			fprintf(stderr, "syntax error\n");
			exit(-1);
		}
		cmd->argv[argc] = mkcopy(q, eq);
		// //////
		// printf("[%d]:%s \t",argc,cmd->argv[argc]);
		// //////
		argc++;
		if (argc >= MAXARGS) {
			fprintf(stderr, "too many args\n");
			exit(-1);
		}

		ret = parseredirs(ret, ps, es);
	}
	cmd->argv[argc] = 0;
	
	return ret;
}

int main(void) {
	static char buf[100];
	int fd, r;

	// Read and run input commands.l
	while (getcmd(buf, sizeof(buf)) >= 0) {
		if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
			// Clumsy but will have to do for now.
			// Chdir has no effect on the parent if run in the child.
			buf[strlen(buf) - 1] = 0; // chop \n
			if (chdir(buf + 3) < 0)
				fprintf(stderr, "cannot cd %s\n", buf + 3);
			continue;
		}
		if (fork1() == 0)
			runcmd(parsecmd(buf));
		wait(&r);
	}
	exit(0);
}




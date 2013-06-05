//--------------------------------------------
// NAME: Blagovest Bojinov
// CLASS: XIb
// NUMBER: 5
// PROBLEM: #5
// FILE NAME: shell2.c (unix file name)
// FILE PURPOSE:
// Implementation of simple command 
// interpretator shell with pipe option.
//---------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#define READ_FD 0
#define WRITE_FD 1


int pipeflag = 0;

struct Job {
	char* command;
	char** argv;
	int stdinput;
	int stdoutput;
};

char** parse_cmdline(const char* cmdline, struct Job* command);
struct Job* split_commands(const char* cmdline);
void striptrailspace(char *line);

int main(int argc, char* argv[]) {
	struct Job* commands;
	char cmdline[1024];
	int execerr;
	int childwait;
	
	int savestdin = dup(STDIN_FILENO);
	int savestdout = dup(STDOUT_FILENO);
	
	int fd[2];
	
	pid_t firstchild;
	pid_t secondchild;
		
	// reads from stdin
	while(gets(cmdline) != NULL) {
		commands = split_commands(cmdline);
		if(pipeflag) {
			pipe(fd);
		}
		
		if((firstchild = fork()) >= 0) {
			if(firstchild == 0) {
				if(pipeflag) {
					close(fd[READ_FD]);
					dup2(fd[WRITE_FD], 1);
					close(fd[WRITE_FD]);	
				}		
		
				execerr = execvp(commands[0].command, commands[0].argv);
			
				// if there is error proccess it
				if(execerr == -1) {
					if(errno == ENOENT)
						printf("%s: No such file or directory\n", commands[0].command);
					if(errno == EACCES)
						printf("%s: Permission denied\n", commands[0].command);
				}
			} else {
				if(pipeflag) {
					waitpid(firstchild, &childwait, 0);
					if((secondchild = fork()) >= 0) {
						if(secondchild == 0) {
							close(fd[WRITE_FD]);
							dup2(fd[READ_FD], 0);
							close(fd[READ_FD]);							
							execerr = execvp(commands[1].command, commands[1].argv);
					
							// if there is error proccess it
							if(execerr == -1) {
								if(errno == ENOENT)
									printf("%s: No such file or directory\n", commands[1].command);
								if(errno == EACCES)
									printf("%s: Permission denied\n", commands[1].command);
							}
				
							// wait for child procces to end
							waitpid(secondchild, &childwait, 0);
						} else {
							close(fd[0]);
							close(fd[1]);
						}
					}
				}
			}
		}
		pipeflag = 0;
		dup2(savestdin, STDIN_FILENO);
		dup2(savestdout, STDOUT_FILENO);
	}

	return 0;
}

//--------------------------------------------
// FUNCTION: striptrailspace
// removes whitespace from end of string
// PARAMETERS:
// char* line
//----------------------------------------------
void striptrailspace(char *line) {
	int i = strlen(line) - 1;
 	
 	while( i > 0 ) {
 		if(isspace(line[i])) {
			line[i] = '\0';
        } else {
			break;
		}
		i--;
	}
}

//--------------------------------------------
// FUNCTION: parse_cmdline
// splits by whitespace intervals the given 
// string to an array of strings
// PARAMETERS:
// const char *cmdline:
// string to be splited
// struct Job* command:
// the structure of the command to split
//----------------------------------------------
char** parse_cmdline(const char* cmdline, struct Job* command) {
	char** result;
	char* token;
	char* cmd = strdup(cmdline);
	int i;
	
	striptrailspace(cmd);
	
	i = 0;
	while ((token = strsep(&cmd, " ")) != NULL) {
		i++;
	}

	cmd = strdup(cmdline);
	result = malloc(sizeof(char*) * i+1);
	
	striptrailspace(cmd);
	
	int indupflag = 0;
	int outdupflag = 0;
		
	i = 0;
	while ((token = strsep(&cmd, " ")) != NULL) {
		if(strcmp(token, "<") == 0) {
			indupflag = 1;
			continue;
		}
		if(strcmp(token, ">") == 0) {
			outdupflag = 1;
			continue;
		}
		if(indupflag) {
			int fd = open(token, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			command->stdinput = fd;
			dup2(fd, STDIN_FILENO);
			close(fd);
			indupflag = 0;
			break;
		}
		if(outdupflag) {
			int fd = open(token, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			command->stdoutput = fd;
			dup2(fd, STDOUT_FILENO);
			close(fd);
			outdupflag = 0;
			break;
		}

		result[i] = token;
		i++;
	}

	return result;
}

//--------------------------------------------
// FUNCTION: split_commands
// splits by | for use by pipe
// PARAMETERS:
// const char *cmdline:
// string to be splited
//----------------------------------------------
struct Job* split_commands(const char* cmdline) {
	struct Job* result;
	char* token;
	char* cmd = strdup(cmdline);
	int i;
	int count;
	
	for(i = 0;i < strlen(cmdline);i++) {
		if(cmdline[i] == '|')
			count++;
	}

	result = malloc(sizeof(struct Job) * count+1);

	i=0;
	token = strtok(cmd, "|");
	while(token) {
		if(i <= count){
			result[i].argv = parse_cmdline(token, &result[i]);
			result[i].command = result[i].argv[0];
			token = strtok(0, " |");
		}
		i++;
	}

	if(i > 1) {
		pipeflag = 1;
	}

	return result;
}

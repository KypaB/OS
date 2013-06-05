//--------------------------------------------
// NAME: Blagovest Bojinov
// CLASS: XIb
// NUMBER: 5
// PROBLEM: #4
// FILE NAME: shell.c 
// FILE PURPOSE:
// Implementation of the unix shell. 
//---------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#define BUFFER_SIZE 100
#define ARR_SIZE 100

int main(int argc, char *argv[]){
    char buffer[BUFFER_SIZE];
    char *args[ARR_SIZE];

    int *ret_status;
    int nargs;
    pid_t pid;
    
    while(1){
        printf(">> ");
        fgets(buffer, BUFFER_SIZE, stdin); //stored into the buffer 
        parse_cmdline(buffer, args, ARR_SIZE, &nargs); 

        if (nargs==0) continue;
        if (!strcmp(args[0], "exit" )) 
	  exit(0);       
        pid = fork();
        if (pid){
            printf("Waiting for child (%d)\n", pid);
            pid = wait(ret_status);
            printf("Child (%d) finished\n", pid);
        } else {
            if(execv(args[0], args) == -1) {
		if(errno == ENOENT) {
			fprintf(stderr ,"%s: %s\n", args[0], strerror(errno));
		}
			
		if(errno == EACCES) {
			fprintf(stderr ,"%s: %s\n", args[0], strerror(errno));
		}                
		exit(1);
            }
        }
    }    
    return 0;
}



//--------------------------------------------
// FUNCTION: parse_cmdline(char *buffer, char** args, int args_size, int *nargs)
// Processes the command line arguments and splits as a space
// PARAMETERS:
// char *buffer, char** args, int args_size, int *nargs
//----------------------------------------------
void parse_cmdline(char *buffer, char** args, int args_size, int *nargs) {
    char *buf_args[args_size]; 
    char **cp;
    char *wbuf;
    int i, j;
    
    wbuf=buffer;
    buf_args[0]=buffer; 
    args[0] =buffer;
    
    for(cp = buf_args; (*cp = strsep(&wbuf, " \n\t")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }
    
    for (j=i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)
            args[j++]=buf_args[i];
    }
    
    *nargs=j;
    args[j]=NULL;
}

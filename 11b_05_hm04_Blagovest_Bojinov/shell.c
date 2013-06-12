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
    char buffer[BUFFER_SIZE];                  //masiw от чарове
    char *args[ARR_SIZE];                      //масив от стригове

    int *ret_status;                           //поинтар
    int nargs;                                 //броя на агументите
    pid_t pid;                                 //създавам нишка
    
    while(1){                                  //безкраен цикъл
        printf(">> ");
        fgets(buffer, BUFFER_SIZE, stdin); //от стандартния вход ще запазим 100 елемта в буфера
        parse_cmdline(buffer, args, ARR_SIZE, &nargs);      //функцията която разделия аргументите

        if (nargs==0) continue;               //ако нямаш аргументи започва нова итерация
        if (!strcmp(args[0], "exit" ))        //ако нулевият аргумент ти е равен на exit 
	  exit(0);       
        pid = fork();                         //създаваме процес
        if (pid){                             //ако сме в бащата
            printf("Waiting for child (%d)\n", pid);    
            pid = wait(ret_status);   //вкарва елса
            printf("Child (%d) finished\n", pid);  //притваш го най-накрая
        } else {                                   //ако си в детето
            if(execv(args[0], args) == -1) {       //ако пускането ти на програмта с 2та аргумента ти върне -1 пускаме проверка за ерори
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
    char *buf_args[args_size];   //масив от стрингове
    char **cp;                   //масив от чарове
    char *wbuf;                  //поинтар 
    int i, j;
    
    wbuf=buffer;                 //копирам буфери
    buf_args[0]=buffer;          
    args[0] =buffer;
    
    for(cp = buf_args; (*cp = strsep(&wbuf, " \n\t")) != NULL ;){        //махам на wbuf спейсове ,нов ред и табулация
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))             //ако срешне терминираща нула или надвиши броя на елемнтите брейква
            break;
    }
    
    for (j=i=0; buf_args[i]!=NULL; i++){                                 //ходи елемт по елемнт
        if(strlen(buf_args[i])>0)                                        //ако дължинта на Iтият елемт е > 0
            args[j++]=buf_args[i];                                       //копира
    }
    
    *nargs=j;                                                            
    args[j]=NULL;                                                        //за execv ни трябва последният елемт на масива да е NULL
}

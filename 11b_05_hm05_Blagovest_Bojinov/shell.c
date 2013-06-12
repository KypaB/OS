//--------------------------------------------
// NAME: Anton Dedikov
// CLASS: XIb
// NUMBER: 4
// PROBLEM: #5
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
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 100
#define ARR_SIZE 100
 
int in_flag = 0;       //флаг  за <
int out_flag = 0;      //флаг за >
int pipe_flag = 0;     //флаг за |

char* out_file_name;
char* in_file_name;

//--------------------------------------------
// FUNCTION: parse_cmdline(char *buffer, char** args, int args_size, int *nargs)
// Processes the command line arguments and splits as a space
// PARAMETERS:
// char *buffer, char** args, int args_size, int *nargs
//----------------------------------------------
void parse_cmdline(char *buffer, char** args, char** args2, int args_size, int *nargs) {    
    char *buf_args[args_size]; 
    char **cp;                   //масив от стрингове
    char *wbuf;                  //масив от чарове
    int i, k, j;
    
    buf_args[0]=buffer; 
    
    for(cp = buf_args; (*cp = strsep(&buffer, " \n\t")) != NULL ;){        //проверява ти за спейсове и табулации и нов ред
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))               // гледа дали има терминираща нула или дали сме превишили броя на елемтите
            break;
    }
    
    for (k=j=i=0; buf_args[i]!=NULL; i++){                                 //ходите по елемнтите по масива и ако има такива символи дига флага

        if(strcmp(buf_args[i], ">") == 0){                                 //ако I съвпадне с >(четене) дига алт флага 
            out_flag = 1;
            out_file_name = buf_args[i+1];                                 //задаваш име на изходният файл
            break;      
        }        

        if(strcmp(buf_args[i], "<") == 0){                                 //като горе ама пише ;дд
            in_flag = 1;
            in_file_name = buf_args[i+1];                
            break;
        }        

        if(strcmp(buf_args[i], "|") == 0){                                 //дига пайп флага 
            pipe_flag = 1;
            continue;
        }        
     
        if( (strlen(buf_args[i])>0) && (strcmp(buf_args[i], ">") != 0) && (strcmp(buf_args[i], "<") != 0) && (strcmp(buf_args[i], "|") != 0)){       //ако има някои от тези 3 символа
            if (pipe_flag == 1){                            //ако е дигнат пайп флага
                args2[k++] = buf_args[i];                   //копираме садържанието на масива буф_аргс
            }else{
                args[j++] = buf_args[i];                    //копираме садържанието на масив буф_аргс
            }
 
        }
    }
    
    *nargs=j;
    args[j]=NULL;                                           //при execv трябва последният елемт да ми е нул 
    args2[k]=NULL;                                          //като горе
}


int main(int argc, char *argv[]){
    char buffer[BUFFER_SIZE];
    char *args[ARR_SIZE];
    char *args2[ARR_SIZE];

    int exec_status;
    int *ret_status;
    int nargs;
    int out;
    int in;
    pid_t pid;
    
    while(1){                                                 //безкраен цикъл
        printf(">> ");
        fgets(buffer, BUFFER_SIZE, stdin); //stored into the buffer 
        parse_cmdline(buffer, args, args2, ARR_SIZE, &nargs); //викаме парса
        
        if (nargs==0) continue;                               //ако нямаме елемнти връщаме се в началото на цикала
        if (!strcmp(args[0], "exit" ))                        //ако 0елемт ни е = на exit exitvame ;д
	        exit(0);       
        pid = fork();                                         //прави процес
        if (pid == -1)                                        //ако ерор го принтим
            perror("fork");
       
        if (pid){                                             //ако си в бащата
            printf("Waiting for child (%d)\n", pid);          //прнтваме това ;дд
            pid = wait(ret_status);                           //отива е елса
            printf("Child (%d) finished\n", pid);             //най-накрая изпринтира това
        } else {
            if (out_flag == 1){                               //ама има флаг за символа 
                out = open(out_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWGRP | S_IWUSR);    //отваряме файла за писане
                dup2(out, 1);                                //пренасочваме стандартният изход към отвореният файл
                close(out);                                  //затваряме отвореният файл 
                exec_status = execv(args[0], args);          //пуска програмта с 2аргумента (името на програмата и аргументите за тая програма която искаме да пуснем)
                exit(0);                                     
                                   
            }else if (in_flag == 1){
                in = open(in_file_name, O_RDONLY, S_IRUSR | S_IWGRP | S_IWUSR); //отваряме файла за четене
                dup2(in, 0);                              //пренасочваме стандартиният вход към отвореният файл
                close(in);
                exec_status = execv(args[0], args);                
                exit(0);
            }else if(pipe_flag == 1){                              //ако флага на пайпа в вдигнат
                
                int fd[2];                                         //файлов дискриптор
                pipe(fd);                                          //ше направи пайп
                pid_t childpid;                                    
      
                childpid = fork();                                 //пускаме процеса
                if (childpid == -1){                               
                   perror("fork");
                }
                if (childpid == 0) {                               //ако си в бащата 
                    close(fd[0]);                                  //затваряш входа
                    dup2(fd[1], 1);                                //пренасочваш стандартния изход към изхода на програмта
                    close(fd[1]);                                  //затваряш изхода
                    execv(args[0], args);                          //пуска програмта с 2та аргумента
                    exit(0);        
                }else{                                             //ако си в сина
                    close(fd[1]);                                  //затваряш изхода
                    dup2(fd[0],0);                                 //пренасочваш сттандартния вход към входа
                    close(fd[0]);                                  //затваряш входа
                    execv(args2[0],args2);                         //пускаш прогата 2 та аргумента
                    exit(0);        
                }          
            }else{                                                  //ако нямаш флагове вдигната флагове
                            
                exec_status = execv(args[0], args);               //пускаш програмата с2та агумента
                exit(0);
                }
         }
            
         if(exec_status == -1) {                                  //ако ти върне -1 обработваш грешките 
               		if(errno == ENOENT) {
                		fprintf(stderr ,"%s: %s\n", args[0], strerror(errno));
                    }
			
		            if(errno == EACCES) {
			            fprintf(stderr ,"%s: %s\n", args[0], strerror(errno));
		            }                
		            exit(1);
          }   
    } 
    return 0;
}

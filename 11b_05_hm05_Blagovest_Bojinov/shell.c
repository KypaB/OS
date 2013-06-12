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
 
int in_flag = 0;       //����  �� <
int out_flag = 0;      //���� �� >
int pipe_flag = 0;     //���� �� |

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
    char **cp;                   //����� �� ���������
    char *wbuf;                  //����� �� ������
    int i, k, j;
    
    buf_args[0]=buffer; 
    
    for(cp = buf_args; (*cp = strsep(&buffer, " \n\t")) != NULL ;){        //��������� �� �� �������� � ��������� � ��� ���
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))               // ����� ���� ��� ����������� ���� ��� ���� ��� ��������� ���� �� ��������
            break;
    }
    
    for (k=j=i=0; buf_args[i]!=NULL; i++){                                 //������ �� ��������� �� ������ � ��� ��� ������ ������� ���� �����

        if(strcmp(buf_args[i], ">") == 0){                                 //��� I �������� � >(������) ���� ��� ����� 
            out_flag = 1;
            out_file_name = buf_args[i+1];                                 //������� ��� �� ��������� ����
            break;      
        }        

        if(strcmp(buf_args[i], "<") == 0){                                 //���� ���� ��� ���� ;��
            in_flag = 1;
            in_file_name = buf_args[i+1];                
            break;
        }        

        if(strcmp(buf_args[i], "|") == 0){                                 //���� ���� ����� 
            pipe_flag = 1;
            continue;
        }        
     
        if( (strlen(buf_args[i])>0) && (strcmp(buf_args[i], ">") != 0) && (strcmp(buf_args[i], "<") != 0) && (strcmp(buf_args[i], "|") != 0)){       //��� ��� ����� �� ���� 3 �������
            if (pipe_flag == 1){                            //��� � ������ ���� �����
                args2[k++] = buf_args[i];                   //�������� ������������ �� ������ ���_����
            }else{
                args[j++] = buf_args[i];                    //�������� ������������ �� ����� ���_����
            }
 
        }
    }
    
    *nargs=j;
    args[j]=NULL;                                           //��� execv ������ ���������� ����� �� �� � ��� 
    args2[k]=NULL;                                          //���� ����
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
    
    while(1){                                                 //�������� �����
        printf(">> ");
        fgets(buffer, BUFFER_SIZE, stdin); //stored into the buffer 
        parse_cmdline(buffer, args, args2, ARR_SIZE, &nargs); //������ �����
        
        if (nargs==0) continue;                               //��� ������ ������� ������� �� � �������� �� ������
        if (!strcmp(args[0], "exit" ))                        //��� 0����� �� � = �� exit exitvame ;�
	        exit(0);       
        pid = fork();                                         //����� ������
        if (pid == -1)                                        //��� ���� �� �������
            perror("fork");
       
        if (pid){                                             //��� �� � ������
            printf("Waiting for child (%d)\n", pid);          //�������� ���� ;��
            pid = wait(ret_status);                           //����� � ����
            printf("Child (%d) finished\n", pid);             //���-������ ���������� ����
        } else {
            if (out_flag == 1){                               //��� ��� ���� �� ������� 
                out = open(out_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWGRP | S_IWUSR);    //�������� ����� �� ������
                dup2(out, 1);                                //������������ ������������ ����� ��� ���������� ����
                close(out);                                  //��������� ���������� ���� 
                exec_status = execv(args[0], args);          //����� ��������� � 2��������� (����� �� ���������� � ����������� �� ��� �������� ����� ������ �� ������)
                exit(0);                                     
                                   
            }else if (in_flag == 1){
                in = open(in_file_name, O_RDONLY, S_IRUSR | S_IWGRP | S_IWUSR); //�������� ����� �� ������
                dup2(in, 0);                              //������������ ������������� ���� ��� ���������� ����
                close(in);
                exec_status = execv(args[0], args);                
                exit(0);
            }else if(pipe_flag == 1){                              //��� ����� �� ����� � �������
                
                int fd[2];                                         //������ ����������
                pipe(fd);                                          //�� ������� ����
                pid_t childpid;                                    
      
                childpid = fork();                                 //������� �������
                if (childpid == -1){                               
                   perror("fork");
                }
                if (childpid == 0) {                               //��� �� � ������ 
                    close(fd[0]);                                  //�������� �����
                    dup2(fd[1], 1);                                //����������� ����������� ����� ��� ������ �� ���������
                    close(fd[1]);                                  //�������� ������
                    execv(args[0], args);                          //����� ��������� � 2�� ���������
                    exit(0);        
                }else{                                             //��� �� � ����
                    close(fd[1]);                                  //�������� ������
                    dup2(fd[0],0);                                 //����������� ������������ ���� ��� �����
                    close(fd[0]);                                  //�������� �����
                    execv(args2[0],args2);                         //������ ������� 2 �� ���������
                    exit(0);        
                }          
            }else{                                                  //��� ����� ������� �������� �������
                            
                exec_status = execv(args[0], args);               //������ ���������� �2�� ��������
                exit(0);
                }
         }
            
         if(exec_status == -1) {                                  //��� �� ����� -1 ���������� �������� 
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

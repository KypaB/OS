//------------------------------------------
// NAME: Daniel Georgiev
// CLASS: 11b
// NUMBER: 11
// PROBLEM: #2
// FILE NAME: wc.c
// FILE PURPOSE:
// wc implementation
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#define IN 1 /* inside a word */
#define OUT 0 /* outside a word */




int optw = 0, optl = 0, optc=0;
//--------------------------------------------
// FUNCTION: checkOpt
// proverqva koi opcii sa aktivirani
// PARAMETERS:
// priema parametrite ot vxoda argc, argv
//----------------------------------------------
int checkOpt(int argc,char *argv[]){            //Минава през всичките аргументи и проверява какви опции са подадени
	int i;
	for(i=1;i<argc;i++){                        //менава през висчките аргументи
		if(argv[i][0] =='-'){                   //сравнява дали е опция аргумента
			int temp;
			for(temp=1; temp < strlen(argv[i]); temp++){   //опхожда агумента за да види колко опции са подадени
				if(argv[i][temp] == 'w') optw=1;           // дали има опция и дига флага
				if(argv[i][temp] == 'l') optl=1;           // *****
				if(argv[i][temp] == 'c') optc=1;
			}
		}
	}
	if(optw==0 && optl==0 && optc==0)           //ако няма опции ги прави и 3те
		optw=optl=optc=1;                       
}
//--------------------------------------------
// FUNCTION: countFile
// broi dumi simvoli i redove ot fail
// PARAMETERS:
// FILE *fp failut koito shte se chete
// char *fileName imeto na faila
//----------------------------------------------
int countFile(FILE *fp, char *fileName){
	int c, nl, nw, nc, state;
	state = OUT;
	nl = nw = nc = 0;

	while ((c = fgetc(fp)) != EOF) {            //взима чарове от файла
		++nc;                                  //увеличава броя на символите
		if (c == '\n')                         //ако е нов ред увиличава редовете
			++nl;
		if (c == ' ' || c == '\n' || c == '\t')      //проверява дали има нов ред или табулация ,ако да прави 
			state = OUT;                             //че си извън дума
		else if (state == OUT) {                     //ако е извън думата и ако не е празно ,нов ред или таб 
			state = IN;                              //прави състояните ,че си в дума 
			++nw;                                    //увеличава броя на думите
		}
	}
	if(optl==1)                                      // ако е дадена опцията L принти редовете
		printf("%d ",nl);
	if(optw==1)                                      //ако е W.....
		printf("%d ",nw);
	if(optc==1)
		printf("%d ",nc);                            // ako e C символи
	printf("%s\n",fileName);
}
//--------------------------------------------
// FUNCTION: count
// broi dumi simvoli i redove ot vxoda
// PARAMETERS:
//
//----------------------------------------------
int count(){
	int nl, nw, nc, state;
	int c;
	state = OUT;
	nl = nw = nc = 0;

	while ((c = getchar()) != EOF) {            //взима чарове от входа
		++nc;
		if (c == '\n')
			++nl;
		if (c == ' ' || c == '\n' || c == '\t')
			state = OUT;
		else if (state == OUT) {
			state = IN;
			++nw;
		}
	}
	if(optl==1)
		printf("%d ",nl);
	if(optw==1)
		printf("%d ",nw);
	if(optc==1)
		printf("%d ",nc);
	printf("-\n");
}


int main(int argc,char *argv[])
{
	checkOpt(argc,argv);       //какви опции са подадени
  //failov deskriptor
  	int f=0;
  	FILE *fp;
  	fp==NULL;
	int n;
	//broqch
	int i;
	//buffer for text
	char l[80];
	//used for permission checking
	struct stat s;
	//check for argument count if 1 read from stdin
	if(argc==1){
		count();
	}else{
	  //foreach file print on the stdout
		for(i=1;i<argc;i++){
			f = 0;
			//get stat of the file i
			if(stat(argv[i], &s)>=0){
				//printf("%d\n",argc);
				//some checks for permision
				if(geteuid()==s.st_uid)
					if(s.st_mode & S_IRUSR)
					f=1;
				else if(getegid()==s.st_gid)
					if(s.st_mode & S_IRGRP)
					f=1;
				else if(s.st_mode & S_IROTH)
					f=1;
				//if f=1 permission denied
				if(!f)
				{
					fprintf(stderr,"wc: %s: Permission denied\n",argv[i]);
					//exit(1);
				}
			}
			//if file name is "-" read from sdtin
			if(strcmp(argv[i],"-")==0){
				count();
			}else{
			  //set file descriptor
			  	if(argv[i][0]!='-'){
					fp=fopen(argv[i],"r");
					//if file doesn't exist
					if(fp==NULL){
						fprintf(stderr,"wc: %s: No such file or directory\n",argv[i]);
					//if file exist print it content 
					}else{
						int chars=0;
						while(fgetc(fp)!=EOF)
							chars++;
						fp=fopen(argv[i],"r");
						countFile(fp,argv[i]);
						fclose(fp);
					}
				}
			}
		}
	}
	
}

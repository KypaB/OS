//--------------------------------------------
// NAME: Blagovest Bojinov
// CLASS: 11 b
// NUMBER: 5
// PROBLEM: #1
// FILE NAME: cat.c (unix file name)
// FILE PURPOSE:
// Reading from files and then
// reading from the command line.
//---------------------------------------------

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void arg_stdin();
void arg_stdout(char* filename);

//--------------------------------------------
// FUNCTION: arg_stdin
// Reads from the input until the end of the
// file and prints what is written.
// PARAMETERS:
// void
//----------------------------------------------
void arg_stdin() {
	char buffer;
	while((buffer=getchar()) != EOF ) {
		printf("%c",buffer);
	}
}

//--------------------------------------------
// FUNCTION: arg_stdout
// Opens a file and reads from it.
// Then prints it's content on the standart output.
// If the file opening isn't successful it prints the
// error .
// If  the filename argument is "-" calls arg_stdin().
// PARAMETERS:
// char* filename :
// path and name of the file to be opened
//----------------------------------------------
void arg_stdout(char* filename) {
	// If the filename is "-" it calls arg_stdin().
	if(strcmp(filename, "-") == 0) {
		arg_stdin();	
	} else {
		// else it opens the file
		int fd=open(filename,O_RDONLY);
		if(fd>0) {
		// if there are no errors occured seeks file size.
			int file_size=lseek(fd,0,SEEK_END);
			lseek(fd,0,SEEK_SET);
			char buffer[file_size];
			// read the file content.
			read(fd,&buffer,file_size);
			buffer[file_size]='\0';	
			// print the content.
			printf("%s", buffer);
		} else {
			// if there are no problems it prints the error
			fprintf(stderr, "cat: %s: %s\n", filename, strerror(errno));
		}
		close(fd);
	}
}

int main(int argc,char *argv[]) {
	// If there is no arguments given it calls arg_stdin.
	if(argc == 1) {
		arg_stdin();
	} else {
		// else it calls arg_stdout.
		int i;
		for(i=1;i<argc;i++) {
			arg_stdout(argv[i]);
		}
	}	
	return 0;
}

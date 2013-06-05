//--------------------------------------------
// NAME: Blagovest Bojinov
// CLASS: 11b
// NUMBER: 5
// PROBLEM: #3
// FILE NAME: ls.c (unix file name)
// FILE PURPOSE:
// Own implementation of the unix command
// 'ls'. Prints the content of directories given as
// arguments. Argument '-l' prints a detailed
// information about the content.
//---------------------------------------------

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int lflag = 0;
int multidirflag = 0;

int main(int argc, char *argv[]) {
	int i;
	for(i = 0; i < argc; i++) {
		if(strcmp(argv[i], "-l") == 0) {
			lflag = 1;		
		}
	}
	
	if((argc-lflag) > 2) {
		multidirflag = 1;
	}

	if((argc-lflag) == 1) {
		getdir(".");
	} else {
		int i;
		for(i = 1;i < argc;i++) {
			getdir(argv[i]);
		}
	}

	return 0;
}

//--------------------------------------------
// FUNCTION: dirselect
// used by scandir to select which directiories
// or files should be added to the return list.
// If directory name is '.', or '..' function
// returns 0 and this directory is skipped.
// PARAMETERS:
// const struct dirent *dp:
// structure which contains information about the
// directory or the file, from here we get its 
// name to open it
//----------------------------------------------
int dirselect(const struct dirent *dp) {
	if(dp->d_name[0] != '.') {
		return 1;
	} else {
		return 0;
	}
}

//--------------------------------------------
// FUNCTION: sumsize
// sums the size in blocks of all directories 
// and files in the directory given as argument
// PARAMETERS:
// char *dirpath:
// path of the directory which content's size
// you want to sum
//----------------------------------------------
int sumsize(char *dirpath) {
	struct dirent **flist;
	struct stat fstat;
	int size = 0;
	int n;
	
	n = scandir(dirpath, &flist, dirselect, alphasort);
	
	if(n > 0) {
		int i;
		for(i = 0; i < n; ++i) {
			char path[50];
			strcpy(path, dirpath);
			strcat(path,"/");
			strcat(path,flist[i]->d_name);
			stat(path, &fstat);
			size += fstat.st_blocks;
		}
	}
	
	return size/2;
}

//--------------------------------------------
// FUNCTION: printdir
// prints the file list given as argument
// the print format depends on lflag, if
// lflag is 0 prints only file names
// if lflag is 1 prints detailed information 
// PARAMETERS:
// void
//----------------------------------------------
void printdir(struct dirent **flist, int length, char *dirpath) {
	int i;
	struct stat fstat;

	if(!lflag) {
		if(multidirflag) {
			printf("%s:\n", dirpath);
		}
		
		for (i = 0; i < length; ++i) {
			printf("%s\n", flist[i]->d_name);
		}
	} else {
		if(multidirflag) {
			printf("%s:\n", dirpath);
		}
		
		printf("total %d\n", sumsize(dirpath));
		
		for(i = 0; i < length; ++i) {	
			char path[50];
			strcpy(path, dirpath);
			strcat(path,"/");
			strcat(path,flist[i]->d_name);
		
			stat(path, &fstat);

			printf((S_ISDIR(fstat.st_mode)) ? "d" : "-");
			printf((fstat.st_mode & S_IRUSR) ? "r" : "-");
			printf((fstat.st_mode & S_IWUSR) ? "w" : "-");
			printf((fstat.st_mode & S_IXUSR) ? "x" : "-");
			printf((fstat.st_mode & S_IRGRP) ? "r" : "-");
			printf((fstat.st_mode & S_IWGRP) ? "w" : "-");
			printf((fstat.st_mode & S_IXGRP) ? "x" : "-");
			printf((fstat.st_mode & S_IROTH) ? "r" : "-");
			printf((fstat.st_mode & S_IWOTH) ? "w" : "-");
			printf((fstat.st_mode & S_IXOTH) ? "x" : "-");

			printf(" %d", (int) fstat.st_nlink);

			printf(" %s", getpwuid(fstat.st_uid)->pw_name);
			
			printf(" %s", getgrgid(fstat.st_gid)->gr_name);

			printf(" %5d", (int) fstat.st_size);
			
			char mtime[20];
			strftime(mtime, 20,"%b  %d %R", localtime(&fstat.st_mtime));	
			printf(" %s", mtime);

			printf(" %s", flist[i]->d_name);

			printf("\n");
		}
	}
}

void getdir(char *dirname) {
	struct dirent **flist;
	int n;
	int file;

	n = scandir(dirname, &flist, dirselect, alphasort);
	
	if(n > 0) {
		printdir(flist, n, dirname);
	} else {
		if(file = fopen(dirname, "r") != NULL) {
			struct stat fstat;
			stat(dirname, &fstat);
			printf("%s\n", dirname);
		}
	
		if(strcmp(dirname, "-l") == 0) {
			return;
		}

		if(errno == ENOENT) {
			fprintf(stderr ,"ls: cannot access %s: %s\n", dirname, strerror(errno));
		}
			
		if(errno == EACCES) {
			fprintf(stderr ,"ls: cannot open directory %s: %s\n", dirname, strerror(errno));
		}
	}
}

//--------------------------------------------
// NAME: Blagovest Bojinov
// CLASS: 11b
// NUMBER: 5
// PROBLEM: #2
// FILE NAME: wc.c
// FILE PURPOSE:
// Code that implements the unix
// function 'wc'. Reading from files and 
// reading from the command line.
//---------------------------------------------
#include<stdio.h>
#include<ctype.h>

enum {OPT_C=4, OPT_W=2, OPT_L=1};

//--------------------------------------------
// FUNCTION: main
// we have some optional arguments like :
//·-c - output to standard output only the number of characters;
//·-w - output to standard output only the number of words;
//·-l - displays the standard output a number of rows.
// PARAMETERS:
// char *argv[]
// int argc
//----------------------------------------------

int main(int argc, char *argv[])
{
	int chars=0;
	int words=0;
	int lines=0;
	int nfiles=0;
	int opt=0;
	int n;
	while((++argv)[0] && argv[0][0]=='-') {
		n=1;
		while(argv[0][n]) {
			switch(argv[0][n++]) {
			case 'c': opt|=OPT_C; break;
			case 'w': opt|=OPT_W; break;
			case 'l': opt|=OPT_L; break;
			default:
				fprintf(stderr, "Unknown option %c\n", argv[0][n-1]);
				fprintf(stderr, "Usage: wc [-cwl] [<filename>*]\n");
				return -1;
			}
		}
	}
	if(!opt) opt=OPT_L|OPT_W|OPT_C;
	while(argv[0]) {
		++nfiles;
		if(wc(*argv, opt, &chars, &words, &lines)==-1) {
			perror(*argv);
			return 1;
		}
		++argv;
	}

	if(nfiles==0) wc("-", opt, &chars, &words, &lines);
	else if(nfiles>1) print("total", opt, chars, words, lines);

	return 0;
}


//--------------------------------------------
// FUNCTION: print
// prints the lines,the words
// the chars and the fname
// PARAMETERS:
// const char *fname
// int opt
// int chars
// int words
// int lines
//----------------------------------------------

int print(const char *fname, int opt, int chars, int words, int lines)
{
	if(opt&OPT_L) printf("% 8d", lines);
	if(opt&OPT_W) printf("% 8d", words);
	if(opt&OPT_C) printf("% 8d", chars);

	if(fname[0]!='-') printf(" %s", fname);

	putchar('\n');

	return 0;
}


//--------------------------------------------
// FUNCTION: wc
// is used to count the characters, words, and re-
//resulted in files. From the command line program receives a list of
//file and the standard output displays the number of characters, words and re-
//resulted in each file and the total number of characters, words and
//lines.
// PARAMETERS:
// списък с параметрите на функцията
// и тяхното значение
//----------------------------------------------

int wc(const char *fname, int opt, int *tot_chars, int *tot_words, int *tot_lines)
{
	int ch;
	int chars=0;
	int words=0;
	int lines=0;
	int sp=1;
	FILE *fp;

	if(fname[0]!='-') fp=fopen(fname, "r");
	else fp=stdin;
	if(!fp) return -1;

	while((ch=getc(fp))!=EOF) {
		++chars;
		if(isspace(ch)) sp=1;
		else if(sp) {
			++words;
			sp=0;
		}
		if(ch=='\n') ++lines;
	}

	print(fname, opt, chars, words, lines);

	if(fname[0]!='-') fclose(fp);

	*tot_chars+=chars;
	*tot_words+=words;
	*tot_lines+=lines;

	return 0;
}




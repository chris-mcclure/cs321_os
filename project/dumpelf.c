//Chris McClure
//CS321 Project
//Last revised: 4/14/19
//Displays the side of sections.text and .data

#include <libelf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/mman.h>

size_t getSection(char);
void printSize(Elf *, size_t);
void initChecks(int, char *[]);

int main(int argc, char *argv[]){
	
	initChecks(argc, argv); // make sure args are in correct format
	
	/****** MAKING SURE ELF VERSION IS UP TO DATE *****/
	if(elf_version(EV_CURRENT) == EV_NONE){
		printf("Incorrect ELF version.\n");
		exit(0);
	} 
	Elf *elf;
	int fp = open(argv[1], O_RDONLY);

	/****** MAKE SURE THE ELF HAS THE MAGIC HEADER ******/
	elf = elf_begin(fp, ELF_C_READ, NULL);
	if(!elf){
		printf("didn't work!\n");
		exit(0);
	}
	char *ehdr_ident = NULL;
	ehdr_ident = elf_getident(elf, NULL);
	if(ehdr_ident[0] == '\x7f' &&
	   ehdr_ident[1] == '\x45' &&	//'E'
	   ehdr_ident[2] == '\x4C' &&	//'L'
	   ehdr_ident[3] == '\x46') {	//'F'
		printf("this is a valid elf object file!\n");
	}
	
	/*PRINTING OUT THE NUMBER OF SECTIONS ACCORDING TO THE HEADER*/
	Elf64_Ehdr *eh64;
	eh64 = elf64_getehdr(elf);
	int shnum = eh64->e_shnum;
	printf("number of sections: %d\n", shnum);

	/**********************************************************/
	/* This prints the correct size of the section in bytes!! */
	size_t num = getSection(*argv[2]);
	printSize(elf, num);
	/**********************************************************/
	return 0;
}

// initChecks
// function takes arguments passed into the command line.
// if there isn't enough arguments passed to the command line, 
// or the file passed isn't an executable, exit with a message. 
void initChecks(int argc, char *argv[]){
	struct stat sb;
	if(stat(argv[1], &sb) == 0 && (sb.st_mode & S_IXUSR) == 0){
		printf("was not passed an executable file. exiting.\n");
		exit(0);
	}
	else if(argc < 3){
		printf("expected 3 arguments. got %d, exiting.\n", argc);
		exit(0);
	}
}


// getSection
// this function takes the paramater, sets and returns the corresponding index to main. otherwise the choice was invalid and program exits.
size_t getSection(char choice){


//find a way to get the index with out manually setting it
	size_t index = 0;
	switch(choice){
	case 'd':
		printf("\nSECTION .DATA\n");
		index = 23;
		break;
	case 't':
		printf("\nSECTION .TEXT\n");
		index = 14;
		break;
	default:
		printf("\nInvalid input, choices are t and d. rebuild and try again\n");
		exit(0);
	}
	return index;
}

// printSize
// function takes the ELF pointer as well as the index returned by getSection. gets the section header of the index and returns the size.
void printSize(Elf *elf, size_t num){
	Elf_Scn * curr = elf_getscn(elf, num);
	Elf64_Shdr *sh64;
	sh64 = elf64_getshdr(curr);
	printf("section size is: %d bytes\n", (int)sh64->sh_size);
}

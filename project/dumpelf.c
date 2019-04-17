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
#include <string.h>

//utility functions for retrieving the size and indices of the elf headers
size_t getSection(char, Elf *, Elf64_Ehdr *);
int getSize(Elf *, size_t);
void initChecks(int, char **);

static inline Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr){
	return (Elf64_Shdr *)((int)hdr + hdr->e_shoff);
}

static inline Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx) {
	Elf64_Shdr * sheader = &elf_sheader(hdr)[idx];
	return sheader;
}

static inline char *elf_str_table(Elf64_Ehdr *hdr){
	if(hdr->e_shstrndx == SHN_UNDEF){
		return NULL;
	}
	Elf64_Shdr *elf_scn = elf_section(hdr, hdr->e_shstrndx);
	return 0;
	
}

static inline char *elf_lookup_string(Elf64_Ehdr *hdr, int offset){
	char *strtab = elf_str_table(hdr);
	if(strtab == NULL){
		return NULL;
	}
	return strtab + offset;
}

//main
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
	
	char * thing = elf_lookup_string(eh64, 4);

	/**********************************************************/
	/* This prints the correct size of the section in bytes!! */
	size_t num = getSection(*argv[2], elf, eh64);
	int size = getSize(elf, num);
	printf("section size is: %d bytes\n", size);
	/**********************************************************/
	
	return 0;
}

// initChecks
// function takes arguments passed into the command line.
// if there isn't enough arguments passed to the command line, 
// or the file passed isn't an executable, exit with a message. 
void initChecks(int argc, char **argv){
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
size_t getSection(char choice, Elf * elf, Elf64_Ehdr * hdr){

// I could use the stackoverflow example to locate a particular 
// section name and then return the index associated with that.
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
	case 'a':
		printf("\nSECTION .DATA + SECTION .TEXT\n");
		int data = getSize(elf, 23);
		int text = getSize(elf, 14);
		printf("combined size is %d bytes\n", data+text);
		exit(0);
	default:
		printf("\nInvalid input, choices are t and d. rebuild and try again\n");
		exit(0);
	}
	return index;
}

// printSize
// function takes the ELF pointer as well as the index returned by getSection. gets the section header of the index and returns the size.
int getSize(Elf *elf, size_t num){
	Elf_Scn * curr = elf_getscn(elf, num);
	Elf64_Shdr *sh64;
	sh64 = elf64_getshdr(curr);
	return (int)sh64->sh_size;
}


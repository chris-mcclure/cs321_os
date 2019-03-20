// Chris McClure
// CS321 Rough Draft
// Purpose: Prints the size of an Elf file that is passed to it.

#include <libelf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char *argv[]){
	/****** MAKING SURE ELF VERSION IS UP TO DATE *****/
	if(elf_version(EV_CURRENT) == EV_NONE){
		printf("NOT CURRENT.\n");
	}
	
	Elf *elf;
	int fp = open(argv[1], O_RDONLY);

	/****** MAKE SURE THE ELF HAS THE MAGIC HEADER ******/
	elf = elf_begin(fp, ELF_C_READ, NULL);
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
	size_t index = 0;
	if(*argv[2] == 'd'){
		printf("section .data\n");
		index = 23;
	}
	else if(*argv[2] == 't'){
		printf("section .text\n");
		index = 14;
	}

	Elf_Scn * curr = elf_getscn(elf, index);

	Elf64_Shdr *sh64;
	sh64 = elf64_getshdr(curr);
	printf("section size is: %d bytes\n", (int)sh64->sh_size);

	/**********************************************************/
	return 0;
}

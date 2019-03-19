# CS321 Project
This project takes an ELF executable and prints the sections .text and .data in bytes.
<ol>

  <li> Create a C program <i> filename </i> or whatever name you'd like that returns an integer and that's it. </li>

  <li> Build an executable with <i> gcc filename.c -o test </i>. </li>

  <li> Build and compile dumpelf.c with  <i> gcc dumpelf.c -lelf -o proj </i> </li>

  <li> dumpelf.c takes an elf executable <i> filename </i> and a letter (d or t) indicating the section to be printed (data or text). </li>

  <li> Run dumpelf.c with <i> ./proj filename d </i>. </li> 

</ol>


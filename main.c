/******************************************************
 * File: main.c
 * Description: This program is the entry point for the 
 *              assembler. It reads input files provided 
 *              as command-line arguments, processes 
 *              them, and performs the pre-processor
 *		as well as thefirst pass of the
 *              assembly.
 ******************************************************/
 
#include "universal.h"
#include "macro.h"
#include "firsttrans.h"
#include "textToBinary.h"

/******************************************************
 * Function: main
 * Description: The entry point of the assembler,
 *		recieves file names from the command line.
 * 
 * @param argc: The number of command-line arguments.
 * @param argv: An array of pointers to the arguments.
 * @return 0 (If there are errors they will be printed but will not terminate)
 ******************************************************/
int main (int argc, char* argv[]) {
  int i;
  FILE *as;
  FILE *am;
  
  /*Check if at least one file is provided*/
  if(argc<2) {
    printf("\nYou didn't enter a file to be read\n");
    exit(0);
  }
  
  /*Process each input file*/
  for (i = 1; i<argc; i++) {
    char fileNameAs[BUFFER];
    char fileNameAm[BUFFER];
    strcpy(fileNameAs, argv[i]);
    strcpy(fileNameAm, argv[i]);
    strcat(fileNameAm, ".am");
    strcat(fileNameAs, ".as");
    
    /*Open the .as file for reading*/
    if (!(as = fopen(fileNameAs, "r"))) {
      printf("\nFATAL ERROR: Cannot open file \"%s\"\n", fileNameAs);
      continue;
    }
    
    if(pre_processor(as, fileNameAm, fileNameAs) == YES) { /*Perform preprocessing and generate the .am file*/
      printf("\nErrors detected in pre processor, output files will not be created\n"); 
      continue;
    }
    else {
      am = fopen(fileNameAm, "r"); /*Open the .am file for reading*/
      firsttrans(am, fileNameAm); /*Execute the first parsing of the code*/
      fclose(am);
    }
  }
  return 0;
}

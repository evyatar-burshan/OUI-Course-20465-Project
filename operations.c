/******************************************************
 * File: operations.c
 * Description: This file contains a function to detect
 *              the opcode index based on the input string.
 ******************************************************/
 
#include "universal.h"

/******************************************************
 * Function: detect_opcode
 * Description: Detects the opcode index based on the input string.
 * 
 * @param input: The input string to detect the opcode.
 * @return The index of the detected opcode, or -1 if not found.
 ******************************************************/
int detect_opcode (char* input) {
  char* opcode[OPCODE] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};
  int i;
  for (i = 0; i<OPCODE; i++) /* Compare the input string with each opcode */
    if(strcmp(opcode[i], input) == 0) return i; /* Return the index if a match is found */
  return -1; /* Return -1 if no match is found */
}

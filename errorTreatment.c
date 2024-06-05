/******************************************************
 * Program: errorTreatment.c
 * Description: This file provides functions to validate
 *              various types of input in the code.
 ******************************************************/

#include "universal.h"

/******************************************************
 * Function: is_number
 * Description: Checks if a string represents a number.
 * 
 * @param input: The input string to check.
 * @return YES if the string represents a number, NO otherwise.
 ******************************************************/
int is_number(char* input) {
  int i;
  for(i = 0; i < strlen(input); i++) /* Iterate through each character in the input string */
    if(!(input[i] == '0' || input[i] == '1' || input[i] == '2' || input[i] == '3' || input[i] == '4' || input[i] == '5' || input[i] == '6' || input[i] == '7' ||
        input[i] == '8' || input[i] == '9' || (i==0 && input[i] == '+') || (i==0 && input[i] == '-')))
      return NO; /* Return NO if a non-numeric character is encountered */
  return YES; /* Return YES if all characters are numeric */
}

/******************************************************
 * Function: is_label
 * Description: Checks if a string represents a label.
 * 
 * @param line: The input string to check.
 * @param h: Pointer to the symbol table.
 * @return Pointer to the label node if found, NULL otherwise.
 ******************************************************/
ptrLabel is_label(char* line, ptrLabel h) {
  while(h) { /* Iterate through the symbol table to find the label */
    if(strcmp(line, h->labelName) == 0) break; 
    h=h->next;
  }
  return h;
}

/******************************************************
 * Function: is_ext_ent
 * Description: Checks if a string represents an external entry.
 * 
 * @param line: The input string to check.
 * @param h: Pointer to the external entry table.
 * @return Pointer to the external entry node if found, NULL otherwise.
 ******************************************************/
ptrExtEnt is_ext_ent(char* line, ptrExtEnt h) {
  while(h) { /* Iterate through the external-entry table to find the entry */
    if(strcmp(line, h->varName) == 0) break;
    h=h->next;
  }
  return h;
}

/******************************************************
 * Function: is_valid_word
 * Description: Checks if a string represents a valid word.
 * 
 * @param word: The input string to check.
 * @return YES if the string represents a valid word, NO otherwise.
 ******************************************************/
int is_valid_word(char* word) {
  const char* savedWords[31] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt", "mcr", "endmcr", ".data", ".string", ".entry", ".extern", ".define", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
  int i;
  if(word == NULL) return NO; /* Return NO if the input word is NULL */
  for(i = 0; i<31;i++) if (strcmp(savedWords[i], word)==0) return NO; /* Check if the word is one of the saved words */
  if(word[0] >= 'A' && word[0] <= 'z') 
    for(i = 1; i<strlen(word);i++) { /* Check if the word starts with a letter and contains valid characters */
    if((word[i] >= 'A' && word[i] <= 'z') || (word[i] >= '0' && word[i] <= '9') || word[i] == '-' || word[i] == '_') {}
    else return NO;
  }
  else return NO;
  return YES;
}

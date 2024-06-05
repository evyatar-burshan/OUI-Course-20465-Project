/******************************************************
 * File: addressingModes.c
 * Description: This file provides functions to detect
 *              and process different addressing modes
 *              used in assembly language.
 ******************************************************/

#include "universal.h"
#include "textToBinary.h"
#include "errorTreatment.h"

char* immediate_addressing(char* input, ptrLabel h);
char* direct_addressing(char* input, ptrLabel h);
char* index_addressing(char* input, ptrLabel h);
char* direct_register_addressing(char* sourceRegister, char* destinationRegister);

/******************************************************
 * Function: detect_addressing_mode
 * Description: Detects the addressing mode based on the input string.
 * 
 * @param input: The input string to detect the addressing mode.
 * @return The detected addressing mode (0-3, or -1 if not detected).
 ******************************************************/
int detect_addressing_mode(char* input) {
  /*Check for "signature marks" of each addressing method and return the addressing mode you found*/
  if(input[0] == '#') return IMMEDIATE;
  else if (input[0] == 'r') return DIRECT_REGISTER;
  else if(strchr(input, '[') != NULL) return INDEX;
  else if(is_number(input) == NO) return DIRECT;
  else return -1; /* Return -1 if no match is found */
}

/******************************************************
 * Function: immediate_addressing
 * Description: Processes immediate addressing mode
 *		and return binary string representing the input.
 * 
 * @param input: The input string representing the immediate value or label.
 * @param h: Pointer to the symbol table.
 * @return The binary string representing the immediate value.
 ******************************************************/
char* immediate_addressing(char* input, ptrLabel h) {
  ptrLabel label;
  if(input[0] == '#') memmove(input, input + 1, strlen(input)); /*Remove '#'*/
  if(is_number(input) == YES) return decimalToBinaryARE(atoi(input), A); /*Check if input is a number*/
  else if ((label = is_label(input, h)) != NULL) { /*Check if input is a label (define) if it isn't a number*/
    if(label->labelType == DEFINE) {
      return decimalToBinaryARE(label->data, A);
    }
  }
  return NULL; /*Return NULL if input does not represent a valid immediate value or label*/
}

/******************************************************
 * Function: direct_addressing
 * Description: Processes direct addressing mode
 *		and return binary string representing the input.
 *
 * @param input: The input string representing the label.
 * @param h: Pointer to the symbol table.
 * @return The binary string representing the address.
 ******************************************************/
char* direct_addressing(char* input, ptrLabel h) {
  ptrLabel label = is_label(input, h);
  if(label == NULL) return NULL; /*Return NULL if label does not exist in symbol table*/
  if(label->labelType == EXTERNAL) return decimalToBinaryARE(0, E); /*If the label is external return binary string with E value for ARE*/
  else if(label->labelType != DEFINE) return decimalToBinaryARE(label->data, R); /*Return binary representation of label's value*/
  return NULL; /*Return NULL if label is not defined*/
}

/******************************************************
 * Function: index_addressing
 * Description: Processes index addressing mode
 *		and return binary string representing the input.
 * 
 * @param input: The input string representing the label and index.
 * @param h: Pointer to the symbol table.
 * @return The binary string(s) representing the address and index.
 ******************************************************/
char* index_addressing(char* input, ptrLabel h) {
  static char output[MAX_WORD*2];
  char* list = strtok(input, "[");
  char* index = strtok(NULL, "]");
  
  if(direct_addressing(list, h) != NULL && is_label(list, h)->labelType != CODE) strcpy(output, direct_addressing(list, h)); /*Copy binary representation of direct addressing result(address of the list)*/
  else return NULL; /*Return NULL if direct addressing fails*/
  strcat(output,"\n");
  if(immediate_addressing(index, h) != NULL) strcat(output, immediate_addressing(index, h)); /*Append binary representation of index addressing result*/
  else return NULL; /*Return NULL if immediate addressing fails*/
  return output; /*Return binary string(s) representing the address and index*/
}

/******************************************************
 * Function: direct_register_addressing
 * Description: Processes direct register addressing mode
 *		and return binary string representing the input.
 * 
 * @parame sourceRegister: The source register (1-7, 0 if NULL).
 * @param destinationRegister: The destination register (1-7, 0 if NULL).
 * @return The binary string representing the registers.
 ******************************************************/
char* direct_register_addressing(char* sourceRegister, char* destinationRegister) {
  if(sourceRegister == NULL)
    return registerToBinary(0, destinationRegister[1]-'0'); /*Return binary representation of destination register*/
  else if(destinationRegister == NULL){
    return registerToBinary(sourceRegister[1]-'0', 0);} /*Return binary representation of source register*/
  else return registerToBinary(sourceRegister[1]-'0', destinationRegister[1]-'0'); /*Return binary representation of both registers*/
  return NULL; /*Return NULL if no valid registers found*/
}

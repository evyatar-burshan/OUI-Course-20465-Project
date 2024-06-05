/******************************************************
 * File: textToBinary.c
 * Description: This file provides functions to convert
 *              decimal numbers, register numbers, and 
 *              operation codes to binary strings.
 ******************************************************/
 
#include "universal.h"

/******************************************************
 * Function: decimalToBinary
 * Description: Converts a decimal number to a binary string.
 * 
 * @param decNum: The decimal number to convert.
 * @return The binary string.
 ******************************************************/
char* decimalToBinary (int decNum) {
  static char binary[MAX_WORD-1];
  int i;
  int mask = 1 << (MAX_WORD-2); /*Mask for extracting bits from the number*/
  
  /*Check if the number is negative and set the sign bit accordingly*/
  if (decNum < 0) {
    decNum &= 0x3FFF;
    binary[0] = '1';
  }
  else binary[0] = '0';
  
  /*Convert the decimal number to binary*/
  for (i = 0; i < MAX_WORD-1; i++) {
    binary[i] = (decNum & mask) ? '1' : '0'; /*Extract the bit and store it in the binary string*/
    mask >>= 1;  /*Shift the mask to the right for the next bit*/
  }
  binary[MAX_WORD-1] = '\0';
  return binary;
}

/******************************************************
 * Function: decimalToBinaryARE
 * Description: Converts a decimal number to a binary string
 *              and appends ARE bits based on the ARE parameter.
 * 
 * @param decNum: The decimal number to convert.
 * @param ARE: The ARE bits to append (A, R, or E).
 * @return The binary string.
 ******************************************************/
char* decimalToBinaryARE (int decNum, int ARE) {
  static char binary[MAX_WORD-1];
  int i;
  int mask = 1 << (MAX_WORD-5); /*Mask for extracting bits from the decimal number*/
  
  /*Check if the number is negative and set the sign bit accordingly*/
  if (decNum < 0) {
    decNum &= 0x3FFF;
    binary[0] = '1';
  }
  else binary[0] = '0';
  
  /*Convert the decimal number to binary*/
  for (i = 1; i < MAX_WORD-3; i++) {
    binary[i] = (decNum & mask) ? '1' : '0';
    mask >>= 1;
  }
  
  /*Append ARE bits based on the ARE parameter*/
  if(ARE == A) {
    binary[MAX_WORD-2] = '0';
    binary[MAX_WORD-3] = '0';
  }
  else if(ARE == R) {
    binary[MAX_WORD-2] = '0';
    binary[MAX_WORD-3] = '1';
  }
  else if(ARE == E) {
    binary[MAX_WORD-2] = '1';
    binary[MAX_WORD-3] = '0';
  }
  
  binary[MAX_WORD-1] = '\0';
  return binary;
}

/******************************************************
 * Function: registerToBinary
 * Description: Converts source and destination register numbers
 *              to a binary string.
 * 
 * @param sourceRegisterNum: The source register number (1-7, 0 if NULL).
 * @param destinationRegisterNum: The destination register number (1-7, 0 if NULL).
 * @return The binary string.
 ******************************************************/
char* registerToBinary (int sourceRegisterNum, int destinationRegisterNum) {
  static char binary[MAX_WORD-1];
  int i;
  int maskSource = 1 << (MAX_WORD-13); /*Mask for extracting bits from the source register number*/
  int maskDestination = 1 << (MAX_WORD-13); /*Mask for extracting bits from the destination register number*/
  
  /*Convert source and destination register numbers to binary*/
  for (i = 0; i < MAX_WORD-1; i++) {
    if( i > 5 && i < 9) {
      binary[i] = (sourceRegisterNum & maskSource) ? '1' : '0'; /*Extract the bit and store it in the binary string*/
      maskSource >>= 1; /*Shift the mask to the right for the next bit*/
    }
    else if( i > 8 && i < 12) {
      binary[i] = (destinationRegisterNum & maskDestination) ? '1' : '0'; /*Extract the bit and store it in the binary string*/
      maskDestination >>= 1; /*Shift the mask to the right for the next bit*/
    }
    else binary[i] = '0';
  }
  
  binary[MAX_WORD-1] = '\0';
  return binary;
}

/******************************************************
 * Function: operationToBinary
 * Description: Converts an operation code and its addressing modes
 *              to a binary string, appending ARE bits based on
 *              the ARE parameter.
 * 
 * @param opcode: The operation code (0-15).
 * @param addressingModeSource: The addressing mode for the source operand (0-3).
 * @param addressingModeDestination: The addressing mode for the destination operand (0-3).
 * @param ARE: The ARE bits to append (A, R, or E).
 * @return The binary string.
 ******************************************************/
char* operationToBinary (int opcode, int addressingModeSource, int addressingModeDestination, int ARE) {
  static char binary[MAX_WORD-1];
  int i;
  int maskOpcode = 1 << (MAX_WORD-12); /*Mask for extracting bits from the operation code */
  int maskSource = 1 << (MAX_WORD-14); /*Mask for extracting bits from the source addressing mode*/
  int maskDestination = 1 << (MAX_WORD-14); /*Mask for extracting bits from the destination addressing mode*/
    
  /*Convert operation code and addressing modes to binary */
  for (i = 0; i < MAX_WORD-3; i++) {
    if( i > 3 && i < 8) {
      binary[i] = (opcode & maskOpcode) ? '1' : '0'; /*Extract the bit and store it in the binary string*/
      maskOpcode >>= 1; /*Shift the mask to the right for the next bit*/
    }
    else if( i > 7 && i < 10) {
      binary[i] = (addressingModeSource & maskSource) ? '1' : '0'; /*Extract the bit and store it in the binary string*/
      maskSource >>= 1; /*Shift the mask to the right for the next bit*/
    }
    else if( i > 9 && i < 12) {
      binary[i] = (addressingModeDestination & maskDestination) ? '1' : '0'; /*Extract the bit and store it in the binary string*/
      maskDestination >>= 1; /*Shift the mask to the right for the next bit*/
    }
    else binary[i] = '0';
  }
  
  /*Append ARE bits based on the ARE parameter*/
  if(ARE == A) {
    binary[MAX_WORD-2] = '0';
    binary[MAX_WORD-3] = '0';
  }
  else if(ARE == R) {
    binary[MAX_WORD-2] = '0';
    binary[MAX_WORD-3] = '1';
  }
  else if(ARE == E) {
    binary[MAX_WORD-2] = '1';
    binary[MAX_WORD-3] = '0';
  }
  
  binary[MAX_WORD-1] = '\0';
  return binary;
}

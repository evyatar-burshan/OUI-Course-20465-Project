/*************************************************************
 * Program: firsttrans.c
 * Description: This program serves as an assembler for a custom
 *              assembly language. It takes an assembly file as 
 *              input and translates it into machine code, this file
 *		deals with creating the symbol table and data image.
 *************************************************************/

#include "universal.h"
#include "textToBinary.h"
#include "operations.h"
#include "addressingModes.h"
#include "secondtrans.h"
#include "errorTreatment.h"

int lineCounterAm = 1; /* Line counter for assembly file */
int IC = 0; /*Instruction counter*/
int DC = 0; /*Data counter*/

int translate_line(char* curLine, ptrLabel *headLabel, ptrDataImg *headDataImg, ptrCodeImg *headCodeImg, char* fileName);
void define_label(ptrLabel **hptr, char* labelName, int labelType, int data);
void build_data_image (ptrDataImg **hptr, int lineNum, int L, char* output);
void build_code_image (ptrCodeImg **hptr, int lineNum, int opcode, int L, char* output);
int translate_code_line (ptrCodeImg **hptr, int opcode, int lineNum, char* line, char* copyCurLine, char* fileName);

/******************************************************
 * Function: firsttrans
 * Description: Performs the first pass of the translation process for the assembly file.
 * 
 * @param am: Pointer to the assembly file.
 * @param fileName: Name of the assembly file.
 ******************************************************/
void firsttrans(FILE* am, char* fileName) {

  ptrLabel headLabel = NULL;
  ptrDataImg headDataImg = NULL;
  ptrCodeImg headCodeImg = NULL;
  char curLine[BUFFER];
  int isError = NO; /* Flag for error detection */
  ptrLabel p1;
  
  while(fgets(curLine, BUFFER-1, am) != NULL) { /*Parse the file line by line*/
    if (curLine[strlen(curLine) - 1] != '\n') { /* Check for line length exceeding the maximum allowed */
      printf("\nLine length exceeded maximum allowed length (80) in line %d (\"%s...\") in file \"%s\"\n", lineCounterAm, curLine, fileName);
      isError = YES;
      continue;
    }
    if(isError != YES) isError = translate_line(curLine, &headLabel, &headDataImg, &headCodeImg, fileName);
    else translate_line(curLine, &headLabel, &headDataImg, &headCodeImg, fileName);
    lineCounterAm++;
    if(IC+DC>MAX_PROGRAM) isError = YES; /* Check if the program size exceeds the maximum limit */
  }
  
  /* Check if errors were detected */
  if(isError == YES) {
    printf("\nErrors detected in first transition, output files will not be created\n");
    return;
  }
  
  /* Adjust labels' data values */
  p1 = headLabel;
  while(p1) {
    if(p1->labelType != DEFINE && p1->labelType != CODE && p1->labelType != EXTERNAL) p1->data = p1->data + 100 + IC;
    p1 = p1->next;
  }
  
  /* Reset file pointer and proceed to the second pass */
  rewind(am);
  secondtrans(am, &headLabel, &headDataImg, &headCodeImg, fileName, IC, DC);
}

/******************************************************
 * Function: translate_line
 * Description: Translates a single line of code from the assembly file.
 * 
 * @param curLine: Current line of code.
 * @param headLabel: Pointer to the head of the label linked list.
 * @param headDataImg: Pointer to the head of the data image linked list.
 * @param headCodeImg: Pointer to the head of the code image linked list.
 * @param fileName: Name of the assembly file.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
int translate_line(char* curLine, ptrLabel *headLabel, ptrDataImg *headDataImg, ptrCodeImg *headCodeImg, char* fileName) {
  char* copyCurLine = malloc(strlen(curLine)+1);
  char* curArg;
  char* labelName = malloc(strlen(curLine)+1);
  char* binaryLine = calloc(MAX_WORD, sizeof(char));
  int isLabel = NO;
  int counter = 0;
  int opcode;
  strcpy(copyCurLine, curLine);
  curArg = strtok(curLine, "\040\t");
  
  /* Parse directives and handle accordingly */
  if(strcmp(curArg, ".define")==0) {
    int value;
    char* valueStr;
    labelName = strtok(NULL, "\040\t=\040\t"); /* Get the defined label name */
    
    /* Check for missing arguments */
    if(labelName==NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    /* Check for duplicate label definitions */
    if(is_label(labelName, *headLabel)!=NULL) {
      printf("\n\"%s\" is defined more than once in line %d: %s in file \"%s\"\n", labelName, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    /* Check for label name length */
    if(strlen(labelName) > MAX_LABEL) {
      printf("\n\"%s\" is longer than %d characters in line %d: %s in file \"%s\"\n", labelName, MAX_LABEL-1, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    /* Get the value assigned to the label */
    valueStr = strtok(NULL,"=\040\t");
    if(valueStr==NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    if(valueStr[strlen(valueStr)-1]=='\n') valueStr[strlen(valueStr)-1]= '\0'; /* Remove newline character from value string */
    /* Check if value is a valid number */
    if(is_number(valueStr) == NO) {
      printf("\n\"%s\" is not a real number in line %d: %s in file \"%s\"\n", valueStr, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    value = atoi(valueStr);
    define_label(&headLabel, labelName, DEFINE, value);
    return NO;
  } 
  /* Handle label definition */
  else if(curArg[strlen(curArg)-1] == ':') {
    isLabel = YES;
    strcpy(labelName, curArg);
    labelName[strlen(labelName)-1] = '\0';
    if(is_valid_word(labelName) == NO) { /* Check if label name is valid */
      printf("\n\"%s\" is not a valid label name in line %d: %s in file \"%s\"\n", labelName, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    if(strlen(labelName) > MAX_LABEL) { /* Check for label name length */
      printf("\n\"%s\" is longer than %d characters in line %d: %s in file \"%s\"\n", labelName, MAX_LABEL-1, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
  }
  /* Handle .extern directive */
  else if(strcmp(curArg, ".extern") == 0) {
    while((curArg = strtok(NULL, "\040\t,\040\t")) != NULL) {
      if(curArg[strlen(curArg)-1]=='\n') curArg[strlen(curArg)-1]= '\0';
      define_label(&headLabel, curArg, EXTERNAL, 0);
    }
    return NO;
  }
  /* Handle .entry directive */
  else if(strcmp(curArg, ".entry") == 0) {
    return NO;
  }
  /* Handle .data directive */
  else if (strcmp(curArg, ".data") == 0) {
    ptrLabel label;
    if(isLabel==YES) {
      define_label(&headLabel, labelName, DATA, DC);
    }
    if((curArg = strtok(NULL, "\040\t,\040\t")) == NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    while(curArg != NULL) {
      if(curArg[strlen(curArg)-1]=='\n') curArg[strlen(curArg)-1]= '\0';
      counter++;
      binaryLine = realloc(binaryLine, (MAX_WORD+1)*counter);
      if(is_number(curArg) == YES) strcat(binaryLine, decimalToBinary(atoi(curArg)));
      else if((label = is_label(curArg, *headLabel)) != NULL && label->labelType == DEFINE) strcat(binaryLine, decimalToBinary(label->data));
      else {
        printf("\n\"%s\" is neither a number nor a symbol(.define value) in line %d: %s in file \"%s\"\n", curArg, lineCounterAm, copyCurLine, fileName);
        return YES;
      }
      if((curArg = strtok(NULL, "\040\t,\040\t")) != NULL) strcat(binaryLine, "\n");
    }
    build_data_image(&headDataImg, DC, counter, binaryLine);
    DC+=counter;
    return NO;
  }
  /* Handle .string directive */
  else if (strcmp(curArg, ".string") == 0) {
    int i;
    char* str;
    curArg = strtok(NULL, "\040\t");
    if(curArg==NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    if(isLabel==YES) {
      define_label(&headLabel, labelName, DATA, DC);
    }
    str = strchr(curArg, '\"');
    if(str[strlen(str)-2] != '\"') {
      printf("\nExtranous text after \" in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    i = 1;
    while(i<strlen(str)-2) {
      counter++;
      if(str[i] == '\n') break;
      if(isprint(str[i]) == 0) {
        printf("\nCannot print\"%c\" (illegal character) in line %d: %s in file \"%s\"\n", str[i], lineCounterAm, copyCurLine, fileName);
        return YES;
      }
      binaryLine = realloc(binaryLine, (MAX_WORD+1)*counter);
      strcat(binaryLine, decimalToBinary(str[i]));
      i++;
      strcat(binaryLine, "\n");
    }
    counter++;
    binaryLine = realloc(binaryLine, (MAX_WORD+1)*counter);
    strcat(binaryLine, decimalToBinary(0));
    build_data_image(&headDataImg, DC, counter, binaryLine);
    DC+=counter;
    return NO;
  }
  /* Handle other arguments */
  else if ((opcode = detect_opcode(curArg)) != -1) {
    if(curArg[strlen(curArg)-1]=='\n') curArg[strlen(curArg)-1]= '\0'; /* Remove newline character if present */
    if((opcode = detect_opcode(curArg)) == -1) { /* If not a valid opcode */
      printf("\n\"%s\" is not a legal operation in line %d: %s in file \"%s\"\n", curArg, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    curArg = strtok(NULL, "\0");
    translate_code_line(&headCodeImg, opcode, IC, curArg, copyCurLine, fileName); /* Translate the code line with the detected opcode */
    return NO;
  }
  
  curArg = strtok(NULL, "\040\t");
  if(curArg==NULL) return NO; /* If no more arguments, return */
  else if(strcmp(curArg, ".define")==0) {
    /* Error: illegal label definition */
    printf("\nIllegal label defenition in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
    return YES;
  }
  else if (strcmp(curArg, ".data") == 0) {
    ptrLabel label;
    if(isLabel==YES) {
      define_label(&headLabel, labelName, DATA, DC); /* Define a label for data section */
    }
    /* Error: missing arguments for .data directive */
    if((curArg = strtok(NULL, "\040\t,\040\t")) == NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    while(curArg != NULL) {
      if(curArg[strlen(curArg)-1]=='\n') curArg[strlen(curArg)-1]= '\0'; /* Remove newline character if present */
      counter++;
      binaryLine = realloc(binaryLine, (MAX_WORD+1)*counter);
      if(is_number(curArg) == YES) strcat(binaryLine, decimalToBinary(atoi(curArg))); /* Convert numbers to binary */
      else if((label = is_label(curArg, *headLabel)) != NULL && label->labelType == DEFINE) strcat(binaryLine, decimalToBinary(label->data)); /* If symbol, get its value */
      /* Error: illegal data argument */
      else {
        printf("\n\"%s\" is neither a number nor a symbol(.define value) in line %d: %s in file \"%s\"\n", curArg, lineCounterAm, copyCurLine, fileName);
        return YES;
      }
      if((curArg = strtok(NULL, "\040\t,\040\t")) != NULL) strcat(binaryLine, "\n");
    }
    build_data_image(&headDataImg, DC, counter, binaryLine); /* Build data image with the binary representation of data */
    DC+=counter; /* Update Data Counter */
    return NO;
  }
  /* Error: missing arguments for .string directive */
  else if (strcmp(curArg, ".string") == 0) {
    int i;
    char* str;
    curArg = strtok(NULL, "\040\t");
    if(curArg==NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    if(isLabel==YES) {
      define_label(&headLabel, labelName, DATA, DC); /* Define a label for string section */
    }
    str = strchr(curArg, '\"');
    /* Error: extraneous text after " */
    if(str[strlen(str)-2] != '\"') {
      printf("\nExtranous text after \" in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    i = 1;
    while(i<strlen(str)-2) {
      counter++;
      if(str[i] == '\n') break;
      /* Error: illegal character in string */
      if(isprint(str[i]) == 0) {
        printf("\nCannot print\"%c\" (illegal character) in line %d: %s in file \"%s\"\n", str[i], lineCounterAm, copyCurLine, fileName);
        return YES;
      }
      binaryLine = realloc(binaryLine, (MAX_WORD+1)*counter);
      strcat(binaryLine, decimalToBinary(str[i])); /* Convert character to binary */
      i++;
      strcat(binaryLine, "\n"); /* Append newline */
    }
    counter++;
    binaryLine = realloc(binaryLine, (MAX_WORD+1)*counter);
    strcat(binaryLine, decimalToBinary(0)); /* Add null terminator */
    build_data_image(&headDataImg, DC, counter, binaryLine); /* Build data image with the binary representation of string */
    DC+=counter; /* Update Data Counter */
    return NO;
  }
  else {
    if(isLabel==YES && is_label(labelName, *headLabel) == NULL) {
      define_label(&headLabel, labelName, CODE, IC+100); /* Define a label for code section if it doesn't already exist */
    }
    if(curArg[strlen(curArg)-1]=='\n') curArg[strlen(curArg)-1]= '\0';
    /* Error: illegal operation */
    if((opcode = detect_opcode(curArg)) == -1) {
      printf("\n\"%s\" is not a legal operation in line %d: %s in file \"%s\"\n", curArg, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    curArg = strtok(NULL, "\0");
    return translate_code_line(&headCodeImg, opcode, IC, curArg, copyCurLine, fileName);
  }
  return NO;
}

/******************************************************
 * Function: define_label
 * Description: Defines a label in the label linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the label linked list.
 * @param labelName: Name of the label to be defined.
 * @param labelType: Type of the label (DEFINE, DATA, CODE, EXTERNAL).
 * @param data: Value associated with the label.
 ******************************************************/
void define_label(ptrLabel **hptr, char* labelName, int labelType, int data) {
  ptrLabel t = (ptrLabel) malloc(sizeof(itemLabel)); /*Create a new item in a linked list*/
  ptrLabel p1, p2;

  if(t==NULL) {
    printf("\nFATAL ERROR: Cannot allocate memory\n");
    exit(0);
  }
  
  /* If the label already exists, update its data or type */
  if((p1 = is_label(labelName, **hptr)) != NULL) {
    if((p1->labelType == CODE && labelType == DATA)) {
      p1->labelType = labelType;
      p1->data = data;
      return;
    }
    else if((p1->labelType == ENTRY && labelType == DATA) || (p1->labelType == ENTRY && labelType == CODE)) {
      p1->data = data;
      return;
    }
  }
  
  t->labelName = malloc(strlen(labelName)+1);
  strcpy(t->labelName, labelName);
  t->labelType = labelType;
  t->data = data;
  
  /* Traverse the linked list to find the end */
  p1 = **hptr;
  while ((p1)) {
    p2 = p1;
    p1 = p1->next;
  }
  
  if (p1 == **hptr) { 
    **hptr = t;
    t->next = p1;
  }
  
  else {
    p2->next = t;
    t->next = p1;
  }
}

/******************************************************
 * Function: build_data_image
 * Description: Builds the data image linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the data image linked list.
 * @param lineNum: Line number associated with the data.
 * @param L: Length of the data.
 * @param output: Binary representation of the data.
 ******************************************************/
void build_data_image (ptrDataImg **hptr, int lineNum, int L, char* output) {
  ptrDataImg t = (ptrDataImg) malloc(sizeof(itemDataImg)); /*Create a new item in a linked list*/
  ptrDataImg p1, p2;

  if(t==NULL) {
    printf("\nCannot allocate memory\n");
    exit(0);
  }
  
  t->lineNum = lineNum;
  t->L = L;
  t->output = malloc(strlen(output)+1);
  strcpy(t->output, output);
  
  /* Traverse the linked list to find the end */
  p1 = **hptr;
  while ((p1)) {
    p2 = p1;
    p1 = p1->next;
  }
  
  if (p1 == **hptr) { 
    **hptr = t;
    t->next = p1;
  }
  
  else {
    p2->next = t;
    t->next = p1;
  }
}

/******************************************************
 * Function: build_code_image
 * Description: Builds the code image linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the code image linked list.
 * @param lineNum: Line number associated with the code.
 * @param opcode: Opcode of the instruction.
 * @param L: Length of the instruction.
 * @param output: Binary representation of the instruction.
 ******************************************************/
void build_code_image (ptrCodeImg **hptr, int lineNum, int opcode, int L, char* output) {
  ptrCodeImg t = (ptrCodeImg) malloc(sizeof(itemCodeImg)); /*Create a new item in a linked list*/
  ptrCodeImg p1, p2;

  if(t==NULL) {
    printf("\nCannot allocate memory\n");
    exit(0);
  }
  
  t->lineNum = lineNum;
  t->L = L;
  t->opcode = opcode;
  t->output = malloc((strlen(output)+1)*L);
  strcpy(t->output, output);
  
  /* Traverse the linked list to find the end */
  p1 = **hptr;
  while ((p1)) {
    p2 = p1;
    p1 = p1->next;
  }
  
  if (p1 == **hptr) { 
    **hptr = t;
    t->next = p1;
  }
  
  else {
    p2->next = t;
    t->next = p1;
  }
}

/******************************************************
 * Function: translate_code_line
 * Description: Translates a line of code into machine code.
 * 
 * @param hptr: Pointer to the pointer to the head of the code image linked list.
 * @param opcode: Opcode of the instruction.
 * @param lineNum: Line number associated with the instruction.
 * @param line: Line of code to be translated.
 * @param copyCurLine: Copy of the current line for error reporting.
 * @param fileName: Name of the assembly file.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
int translate_code_line (ptrCodeImg **hptr, int opcode, int lineNum, char* line, char* copyCurLine, char* fileName) {
  if (opcode == 14 || opcode == 15) { /* Check if the opcode requires no parameters */
    if (line == NULL) {
      build_code_image(hptr, lineNum, opcode, 1, operationToBinary (opcode, 0, 0, A));
      IC+=1;
      return NO;
    }
    else {
      printf("\nExtranous text in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
   }
  }
  else if ((opcode >= 7 && opcode <= 13) || opcode == 4 || opcode == 5) { /* Check if the opcode requires 1 parameters */
    int addressingMode;
    int L;
    if(line == NULL) {
      printf("\nMissing argument in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    addressingMode = detect_addressing_mode(line);
    /* Check validity of addressing modes */
    if (addressingMode == -1) {
      printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", line, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    else if (addressingMode == INDEX) L = 3;
    else if (addressingMode == IMMEDIATE && opcode != 12) {
      printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", line, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    else if (addressingMode == INDEX && (opcode == 9 || opcode == 10 || opcode == 13)) {
      printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", line, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    else L = 2;
    build_code_image(hptr, lineNum, opcode, L, operationToBinary(opcode, 0, addressingMode, A));
    IC+=L;
    return NO;
  }
  else if (opcode <=3 || opcode == 6) { /* Check if the opcode requires 2 parameters */
    char* arg1 = malloc(strlen(line)+1);
    char* arg2 = malloc(strlen(line)+1);
    int addressingModeSource;
    int addressingModeDestination;
    int L = 1;
    arg1 = strtok(line, "\040\t,\040\t");
    arg2 = strtok(NULL, ",\040\t");
    
    if (arg1 == NULL || arg2 == NULL) {
      printf("\nMissing argument(s) in line %d: %s in file \"%s\"\n", lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    
    addressingModeSource = detect_addressing_mode(arg1);
    addressingModeDestination = detect_addressing_mode(arg2);
    /* Check validity of addressing modes */
    if(addressingModeSource == -1) {
      printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", arg1, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    if(addressingModeDestination == -1) {
      printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", arg2, lineCounterAm, copyCurLine, fileName);
      return YES;
    }
    
    if(addressingModeSource == DIRECT_REGISTER && addressingModeDestination == DIRECT_REGISTER) L = 2;
    else {
      if (addressingModeSource == INDEX) L+=2;
      else if ((addressingModeSource == IMMEDIATE || addressingModeSource == DIRECT_REGISTER) && opcode == 6) {
        printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", arg1, lineCounterAm, copyCurLine, fileName);
        return YES;
      }
      else L++;
      if (addressingModeDestination == INDEX) L+=2;
      else if (addressingModeDestination == IMMEDIATE && opcode != 1) {
        printf("\n\"%s\" is an illegal argument in line %d: %s in file \"%s\"\n", arg2, lineCounterAm, copyCurLine, fileName);
        return YES;
      }
      else L++;
    }
    build_code_image(hptr, lineNum, opcode, L, operationToBinary (opcode, addressingModeSource, addressingModeDestination, A));
    IC+=L;
    return NO;
  }
  return YES;
}

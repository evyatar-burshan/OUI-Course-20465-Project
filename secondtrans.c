#include "universal.h"
#include "addressingModes.h"
#include "textToBinary.h"
#include "exportFiles.h"
#include "errorTreatment.h"

int treat_line(char* curLine, ptrLabel *headLabel, ptrDataImg *headDataImg, ptrCodeImg p1, ptrExtEnt *headExtEnt, char* fileName);
int build_operands (ptrCodeImg pointerCode, ptrLabel headLabel, ptrExtEnt **headExtEnt, int opcode, char* line);
void build_ext_ent (ptrExtEnt **hptr, int lineNum, int type, char* varName);
void freeLabel(ptrLabel * hptr);

int lineCounterOb = 1; /* Initializing line counter for the output file */

/******************************************************
 * Function: secondtrans
 * Description: Performs the second pass of the assembly process, generating the output files.
 * 
 * @param am: Pointer to the modified assembly file.
 * @param headLabel: Pointer to the pointer to the head of the label linked list.
 * @param headDataImg: Pointer to the pointer to the head of the data image linked list.
 * @param headCodeImg: Pointer to the pointer to the head of the code image linked list.
 * @param fileName: Name of the output files.
 * @param IC: Value of the Instruction Counter.
 * @param DC: Value of the Data Counter.
 ******************************************************/
void secondtrans(FILE* am, ptrLabel *headLabel, ptrDataImg *headDataImg, ptrCodeImg *headCodeImg, char* fileName, int IC, int DC) {
  char curLine[BUFFER];
  int isError = NO;
  int returnTreatLine;
  ptrCodeImg p1 = *headCodeImg;
  ptrExtEnt headExtEnt = NULL;
  	
  while((fgets(curLine, BUFFER-1, am) != NULL) && p1 != NULL) { /*Parse the file line by line*/
    if (curLine[strlen(curLine) - 1] != '\n') continue;
    if((returnTreatLine = treat_line(curLine, headLabel, headDataImg, p1, &headExtEnt, fileName)) != -1) {
      if(isError != YES) isError = returnTreatLine;
      p1 = p1->next; /* Move to the next node in the code image linked list */
    }
    lineCounterOb++;
  }
  freeLabel(headLabel); /* Free allocated memory for the label linked list */
  if(isError == YES) {
    printf("\nErrors detected in second transition, output files will not be created\n");
    return;
  }
  else export_files(headDataImg, headCodeImg, &headExtEnt, fileName, IC, DC);
}

/******************************************************
 * Function: treat_line
 * Description: Treats each line of the modified assembly file, handling directives and operands.
 * 
 * @param curLine: Current line of the modified assembly file being processed.
 * @param headLabel: Pointer to the pointer to the head of the label linked list.
 * @param headDataImg: Pointer to the pointer to the head of the data image linked list.
 * @param p1: Pointer to the current node in the code image linked list.
 * @param headExtEnt: Pointer to the pointer to the head of the external entries linked list.
 * @param fileName: Name of the output files.
 * @return: int indicating whether an error occurred (YES) or not (-1).
 ******************************************************/
int treat_line(char* curLine, ptrLabel *headLabel, ptrDataImg *headDataImg, ptrCodeImg p1, ptrExtEnt *headExtEnt, char* fileName) {
  char* curArg;
  char* copyCurLine = malloc(strlen(curLine)+1);
  strcpy(copyCurLine, curLine);
  curArg = strtok(curLine, "\040\t");
  
  /* Check if the argument is missing */
  if(curArg == NULL) return -1;
  /* Check if the directive is ".entry" */
  if(strcmp(curArg, ".entry") == 0) {
    ptrLabel label;
    curArg = strtok(NULL, "\040\t");
    /* Check if the argument is missing */
    if(curArg==NULL) {
      printf("\nMissing arguments in line %d: %s in file \"%s\"\n", lineCounterOb, copyCurLine, fileName);
      return YES;
    }
    /* Check if the label is defined twice */
    if(is_ext_ent(curArg, *headExtEnt) != NULL) {
      printf("\n\"%s\" is defined twice in line %d: %s in file \"%s\"\n", curArg, lineCounterOb, copyCurLine, fileName);
      return YES;
    }
    if(curArg[strlen(curArg)-1 == '\n']) curArg[strlen(curArg)-1] = '\0';
    if((label = is_label(curArg, *headLabel)) != NULL) build_ext_ent(&headExtEnt, label->data, ENTRY, curArg);
    else {
      printf("\n\"%s\" is not defined and therefore cannot be entry in line %d: %s in file \"%s\"\n", curArg, lineCounterOb, copyCurLine, fileName);
      return YES;
    }
    return -1;
  }
  else if(strcmp(curArg, ".define")==0 || strcmp(curArg, ".extern") == 0 || strcmp(curArg, ".data") == 0 || strcmp(curArg, ".string") == 0)
    return -1;
  else if(curArg[strlen(curArg)-1] == ':') curArg = strtok(NULL, "\040\t");
  if(strcmp(curArg, ".define")==0 || strcmp(curArg, ".extern") == 0 || strcmp(curArg, ".data") == 0 || strcmp(curArg, ".string") == 0 || strcmp(curArg, ".entry") == 0) return -1;
  else {
    curArg = strtok(NULL, "\0");
    return build_operands (p1, *headLabel, &headExtEnt, p1->opcode, curArg);
  }
}

/******************************************************
 * Function: build_operands
 * Description: Builds the operands of the current code line based on its opcode and addressing modes.
 * 
 * @param pointerCode: Pointer to the current node in the code image linked list.
 * @param headLabel: Pointer to the head of the label linked list.
 * @param headExtEnt: Pointer to the pointer to the head of the external entries linked list.
 * @param opcode: Opcode of the instruction.
 * @param line: Line containing the operands.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
int build_operands (ptrCodeImg pointerCode, ptrLabel headLabel, ptrExtEnt **headExtEnt, int opcode, char* line) {
  if (line == NULL) { /* Checking if the line is empty */
    if (opcode == 14 || opcode == 15) { /* Handling cases where no operands are needed */
      return NO;
    }
  }
  else if ((opcode >= 7 && opcode <= 13) || opcode == 4 || opcode == 5) { /* Processing instructions that have single operands */
    int addressingMode;
    ptrLabel label;
    if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
    addressingMode = detect_addressing_mode(line);
    strcat(pointerCode->output, "\n");
    
    if(addressingMode == DIRECT) {
      strcat(pointerCode->output, direct_addressing(line, headLabel));
      if((label = is_label(line, headLabel)) != NULL && label->labelType == EXTERNAL) build_ext_ent (headExtEnt, pointerCode->lineNum+101, EXTERNAL, line);
      else if((label = is_label(line, headLabel)) != NULL && label->labelType == ENTRY) build_ext_ent (headExtEnt, pointerCode->lineNum+101, ENTRY, line);
    }
    
    else if(addressingMode == IMMEDIATE) strcat(pointerCode->output, immediate_addressing(line, headLabel));
    
    else if(addressingMode == INDEX) {
      strcat(pointerCode->output, index_addressing(line, headLabel));
      line = strtok(line, "\040\t[\040\t");
      if((label = is_label(line, headLabel)) != NULL && label->labelType == EXTERNAL) build_ext_ent (headExtEnt, pointerCode->lineNum+101, EXTERNAL, line);
      else if((label = is_label(line, headLabel)) != NULL && label->labelType == ENTRY) build_ext_ent (headExtEnt, pointerCode->lineNum+101, ENTRY, line);
    }
    
    else if(addressingMode == DIRECT_REGISTER) strcat(pointerCode->output, direct_register_addressing(NULL, line));
    
    return NO;
  }
  else if (opcode <=3 || opcode == 6) { /* Processing instructions that have two operands */
    char* arg1 = malloc(strlen(line)+1);
    char* arg2 = malloc(strlen(line)+1);
    int addressingModeSource;
    int addressingModeDestination;
    ptrLabel label;
    arg1 = strtok(line, "\040\t,\040\t");
    arg2 = strtok(NULL, ",\040\t\n");
    
    addressingModeSource = detect_addressing_mode(arg1);
    addressingModeDestination = detect_addressing_mode(arg2);
    strcat(pointerCode->output, "\n");
    
    /* Source operand processing */
    if(addressingModeSource == DIRECT) {
      strcat(pointerCode->output, direct_addressing(arg1, headLabel));
      if((label = is_label(arg1, headLabel)) != NULL && label->labelType == EXTERNAL) build_ext_ent (headExtEnt, pointerCode->lineNum+101, EXTERNAL, arg1);
      else if((label = is_label(arg1, headLabel)) != NULL && label->labelType == ENTRY) build_ext_ent (headExtEnt, pointerCode->lineNum+101, ENTRY, arg1);
    }
    
    else if(addressingModeSource == IMMEDIATE) strcat(pointerCode->output, immediate_addressing(arg1, headLabel));
    
    else if(addressingModeSource == INDEX) {
      strcat(pointerCode->output, index_addressing(arg1, headLabel));
      arg1 = strtok(arg1, "\040\t[\040\t");
      if((label = is_label(arg1, headLabel)) != NULL && label->labelType == EXTERNAL) build_ext_ent (headExtEnt, pointerCode->lineNum+101, EXTERNAL, arg1);
      else if((label = is_label(arg1, headLabel)) != NULL && label->labelType == ENTRY) build_ext_ent (headExtEnt, pointerCode->lineNum+101, ENTRY, arg1);
    }
    
    else if(addressingModeSource == DIRECT_REGISTER) {
      if(addressingModeDestination == DIRECT_REGISTER) {
        strcat(pointerCode->output, direct_register_addressing(arg1, arg2));
        return NO;
      }
      else {
        strcat(pointerCode->output, direct_register_addressing(arg1, NULL));
      }
    }

    strcat(pointerCode->output, "\n");
    /* Destination operand processing */
    if(addressingModeDestination == DIRECT) {
      strcat(pointerCode->output, direct_addressing(arg2, headLabel));
      if((label = is_label(arg2, headLabel)) != NULL && label->labelType == EXTERNAL) build_ext_ent (headExtEnt, pointerCode->lineNum+101, EXTERNAL, arg2);
      else if((label = is_label(arg2, headLabel)) != NULL && label->labelType == ENTRY) build_ext_ent (headExtEnt, pointerCode->lineNum+101, ENTRY, arg2);
    }
    
    else if(addressingModeDestination == IMMEDIATE) strcat(pointerCode->output, immediate_addressing(arg2, headLabel));
    
    else if(addressingModeDestination == INDEX) {
      strcat(pointerCode->output, index_addressing(arg2, headLabel));
      arg2 = strtok(arg2, "\040\t[\040\t");
      if((label = is_label(arg2, headLabel)) != NULL && label->labelType == EXTERNAL) build_ext_ent (headExtEnt, pointerCode->lineNum+101, EXTERNAL, arg2);
      else if((label = is_label(arg2, headLabel)) != NULL && label->labelType == ENTRY) build_ext_ent (headExtEnt, pointerCode->lineNum+101, ENTRY, arg2);
    }
    
    else if(addressingModeDestination == DIRECT_REGISTER && addressingModeSource != DIRECT_REGISTER) strcat(pointerCode->output, direct_register_addressing(NULL, arg2));
    
    return NO;
  }
  return YES;
}

/******************************************************
 * Function: build_ext_ent
 * Description: Builds external entries and adds them to the external entries linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the external entries linked list.
 * @param lineNum: Line number associated with the external entry.
 * @param type: Type of the external entry (ENTRY or EXTERNAL).
 * @param varName: Name of the external entry.
 ******************************************************/
void build_ext_ent (ptrExtEnt **hptr, int lineNum, int type, char* varName) {
  ptrExtEnt t = (ptrExtEnt) malloc(sizeof(itemExtEnt)); /*Create a new item in a linked list*/
  ptrExtEnt p1, p2;

  if(t==NULL) {
    printf("\nCannot allocate memory\n");
    exit(0);
  }
  
  t->lineNum = lineNum;
  t->type = type;
  t->varName = malloc(strlen(varName)+1);
  strcpy(t->varName, varName);
  
  /* Traverse the linked list to find the end */
  p1 = **hptr;
  while ((p1)&&p1->lineNum<lineNum) {
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
 * Function: freeLabel
 * Description: Frees allocated memory of the label linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the label linked list.
 ******************************************************/
void freeLabel(ptrLabel * hptr) {
  ptrLabel p;
  
  /*Parse the linked list and free each item and its content*/
  while(*hptr) { 
    p = *hptr;
    *hptr = (*hptr)->next;
    free(p->labelName);
    free(p);
  }
}

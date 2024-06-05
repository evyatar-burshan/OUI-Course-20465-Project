#include "universal.h"
#include "errorTreatment.h"

/* Structure definition for a linked list node */
typedef struct node* ptr;
typedef struct node {
  char* macroName;
  char* macro;
  ptr next;
} item;

int handle_line(char* curLine, ptr *h, FILE *as, FILE *am, char* fileName);
ptr add2list(ptr **hptr, char* macroName);
void addMacro(ptr *hptr, char* macro);
void freelist(ptr * hptr);
ptr is_line_macro(char* line, ptr h);

int lineCounterAs = 1;

/******************************************************
 * Function: pre_processor
 * Description: Preprocesses the assembly file, handling macros and generating a modified assembly file.
 * 
 * @param as: Pointer to the original assembly file.
 * @param fileNameAm: Name of the macro file to be generated.
 * @param fileNameAs: Name of the modified assembly file to be generated.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
int pre_processor (FILE *as, char* fileNameAm, char* fileNameAs) {
  FILE *am;
  ptr h = NULL;
  char curLine[BUFFER];
  int i;
  int isError = NO;
  
  am = fopen(fileNameAm, "w"); /* Open the macro file for writing */
  
  while(fgets(curLine, BUFFER-1, as) != NULL) { /*Parse the file line by line*/
    if(curLine[0] == ';') continue; /*Check for comment*/
    if (strcmp(curLine, "\n") == 0) continue; /*Check for empty line*/
    /* Remove leading and trailing whitespaces */
    for(i = 0; isspace(curLine[i]); i++) memmove(curLine, curLine + 1, strlen(curLine));
    for(i = strlen(curLine)-1; isspace(curLine[i]); i--) curLine[strlen(curLine)-1] = '\0';
    if(isError != YES) isError = handle_line(curLine, &h, as, am, fileNameAs); /* Process the line and handle any errors */
    lineCounterAs++;
  }
  fclose(am);
  return isError;
}

/******************************************************
 * Function: handle_line
 * Description: Handles each line of the assembly file, identifying macros and copying lines to the modified assembly file.
 * 
 * @param curLine: Current line of the assembly file being processed.
 * @param h: Pointer to the head of the macro linked list.
 * @param as: Pointer to the original assembly file.
 * @param am: Pointer to the modified assembly file.
 * @param fileNameAs: Name of the modified assembly file.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
int handle_line(char* curLine, ptr *h, FILE *as, FILE *am, char* fileNameAs) {
  char* macro = calloc(BUFFER, sizeof(char)); /*note to self check for memory*/
  char* macroName = malloc(MAX_LABEL);
  char* copyCurLine = malloc(strlen(curLine)+1);
  ptr returnMacro;
  
  strcpy(copyCurLine, curLine); /* Make a copy of the current line */
  
  /* Check for memory allocation errors */
  if(macro == NULL || macroName == NULL) {
    printf("\nFATAL ERROR: Cannot allocate memory\n");
    return YES;
  }
  
  /* Check if the line matches a macro */
  if ((returnMacro = is_line_macro(curLine, *h))!=NULL) {
    fprintf(am, "%s\n", returnMacro->macro);
  }
  
  /* Check if the line defines a new macro */
  else if (curLine[0] == 'm' && curLine[1] == 'c' && curLine[2] == 'r') {
    int i;
    ptr returnMacro;
    macroName = strtok(curLine, "\040\t");
    if(macroName == NULL) {
      printf("\nMissing macro name in line %d: %s in file \"%s\"\n", lineCounterAs, copyCurLine, fileNameAs);
      return YES;
    }
    macroName = strtok(NULL, "\0");
    if(macroName == NULL) {
      printf("\nMissing macro name in line %d: %s in file \"%s\"\n", lineCounterAs, copyCurLine, fileNameAs);
      return YES;
    }
    if(is_valid_word(macroName) == NO) {
      printf("\n\"%s\" is not a valid macro name in line %d: %s in file \"%s\"\n", macroName, lineCounterAs, copyCurLine, fileNameAs);
      return YES;
    }
    returnMacro = add2list(&h, macroName);
    while((fgets(curLine, BUFFER-1, as) != NULL)) {
      if(curLine[0] == ';') continue; /*Check for comment*/
      if (curLine[strlen(curLine) - 1] != '\n') {
        printf("\n\"%s\" Line length exceeded maximum allowed length (80) in line %d: %s in file \"%s\"\n", curLine, lineCounterAs, copyCurLine, fileNameAs);
        return YES;
      }
      if (strcmp(curLine, "\n") == 0) continue; /*Check for empty line*/
      for(i = 0; isspace(curLine[i]); i++) memmove(curLine, curLine + 1, strlen(curLine));
      for(i = strlen(curLine)-1; isspace(curLine[i]); i--) curLine[strlen(curLine)-1] = '\0';
      if (strcmp(curLine, "endmcr") == 0) break;
      strcat(macro, curLine);
      strcat(macro, "\n");
    }
    macro[strlen(macro)-1] = '\0';
    addMacro(&returnMacro, macro);
  }
  
  else { /* No macro found, copy the line to the output file */
    fprintf(am, "%s\n", curLine);
  }
  return NO;
}

/******************************************************
 * Function: is_line_macro
 * Description: Checks if a line from the assembly file corresponds to a defined macro.
 * 
 * @param line: Line from the assembly file being checked.
 * @param h: Pointer to the head of the macro linked list.
 * @return: Pointer to the node containing the macro if found, otherwise NULL.
 ******************************************************/
ptr is_line_macro(char* line, ptr h) {
  while(h) {
    if(strcmp(line, h->macroName) == 0) break;
    h=h->next;
  }
  return h;
}

/******************************************************
 * Function: add2list
 * Description: Adds a macro name to the macro linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the macro linked list.
 * @param macroName: Name of the macro to be added.
 * @return: Pointer to the newly added node.
 ******************************************************/
ptr add2list(ptr **hptr, char* macroName) {
  ptr t = (ptr) malloc(sizeof(item)); /*Create a new item in a linked list*/
  ptr p1, p2;

  if(t==NULL) {
    printf("\nFATAL ERROR: Cannot allocate memory\n");
    exit(0);
  }
  
  t->macroName = malloc(strlen(macroName)+1);
  strcpy(t->macroName, macroName);
  
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
  
  return t;
}

/******************************************************
 * Function: addMacro
 * Description: Adds macro content to the corresponding node in the macro linked list.
 * 
 * @param hptr: Pointer to the node in the macro linked list.
 * @param macro: Content of the macro to be added.
 ******************************************************/
void addMacro(ptr *hptr, char* macro) {
  (*hptr)->macro = malloc(strlen(macro)+1);
  strcpy((*hptr)->macro, macro);
}

/******************************************************
 * Function: freelist
 * Description: Frees allocated memory of the macro linked list.
 * 
 * @param hptr: Pointer to the pointer to the head of the macro linked list.
 ******************************************************/
void freelist(ptr * hptr) {
  ptr p;
  
  /*Parse the linked list and free each item and its content*/
  while(*hptr) { 
    p = *hptr;
    *hptr = (*hptr)->next;
    free(p);
  }
}

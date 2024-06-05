/******************************************************
 * File: exportFfiles.c
 * Description: This file provides functions to export
 *              data and code segments, as well as external
 *              and entry symbols, to output files in a 
 *              specified format.
 ******************************************************/

#include "universal.h"

char* encrypt(char* input);
void freeDataImg(ptrDataImg * hptr);
void freeCodeImg(ptrCodeImg * hptr);
void freeExtEnt(ptrExtEnt * hptr);

/******************************************************
 * Function: export_files
 * Description: Exports data and code segments, as well as external and entry symbols, to output files.
 * 
 * @param headDataImg: Pointer to the head of the data image linked list.
 * @param headCodeImg: Pointer to the head of the code image linked list.
 * @param headExtEnt: Pointer to the head of the external/entry symbol linked list.
 * @param fileName: Name of the output file.
 * @param IC: Instruction counter.
 * @param DC: Data counter.
 ******************************************************/
void export_files(ptrDataImg *headDataImg, ptrCodeImg *headCodeImg, ptrExtEnt *headExtEnt, char* fileName, int IC, int DC) {
  FILE* ob;
  FILE* ext = NULL;
  FILE* ent = NULL;
  int lineCounter = 100;
  int isExt = NO;
  int isEnt = NO;
  ptrExtEnt pointerExtEnt = (*headExtEnt);
  ptrCodeImg pointerCodeImg = (*headCodeImg);
  ptrDataImg pointerDataImg = (*headDataImg);
  char* fileNameExt = calloc(strlen(fileName)+2, sizeof(char));
  
  /* Create file names for external and entry files */
  strcpy(fileNameExt, fileName);
  fileName[strlen(fileName)-2] = 'o';
  fileName[strlen(fileName)-1] = 'b';
  fileNameExt[strlen(fileNameExt)-2] = 'e';
  fileNameExt[strlen(fileNameExt)-1] = 'x'; 
  fileNameExt[strlen(fileNameExt)] = 't';
  fileNameExt[strlen(fileNameExt)+1] = '\0';
  ob = fopen(fileName, "w");  /* Open output file */
  
  fprintf(ob, "  %d %d\n", IC, DC); /* Write IC and DC to output file */
  /* Write code segment to output file */
  if(pointerCodeImg) fprintf(ob, "%04d ", lineCounter);
  while(pointerCodeImg) {
    int i;
    char chars[3]; /* Array to hold binary data */
    chars[2] = '\0';
    for(i = 0;i<strlen(pointerCodeImg->output);i++) {
      if(pointerCodeImg->output[i] == '\n') {
        lineCounter++;
        fprintf(ob, "\n%04d ", lineCounter);
        continue;
      }
      chars[0] = pointerCodeImg->output[i];
      chars[1] = pointerCodeImg->output[i+1];
      fprintf(ob, "%s", encrypt(chars));
      i++;
    }
    pointerCodeImg = pointerCodeImg->next;
    lineCounter++;
    if(pointerCodeImg!=NULL) fprintf(ob, "\n%04d ", lineCounter);
  }
  
  /* Write data image to output file */
  if(pointerDataImg) fprintf(ob, "\n%04d ", lineCounter);
  while(pointerDataImg) {
    int i;
    char chars[3]; /* Array to hold binary data */
    chars[2] = '\0';
    for(i = 0;i<strlen(pointerDataImg->output);i++) {
      if(pointerDataImg->output[i] == '\n') {
        lineCounter++;
        fprintf(ob, "\n%04d ", lineCounter);
        continue;
      }
      chars[0] = pointerDataImg->output[i];
      chars[1] = pointerDataImg->output[i+1];
      fprintf(ob, "%s", encrypt(chars));
      i++;
    }
    pointerDataImg = pointerDataImg->next;
    if(pointerDataImg!=NULL) {
      lineCounter++;
      fprintf(ob, "\n%04d ", lineCounter);
    }
  }
  
  /* Create external and entry files */
  while(pointerExtEnt) {
    if(pointerExtEnt->type == EXTERNAL && isExt == NO) {
      fileNameExt[strlen(fileNameExt)-2] = 'x';
      ext = fopen(fileNameExt, "w");
      isExt = YES;
    }
    if(pointerExtEnt->type == ENTRY && isEnt == NO) {
      fileNameExt[strlen(fileNameExt)-2] = 'n';
      ent = fopen(fileNameExt, "w");
      isEnt = YES;
    }
    if(isExt == YES && isEnt == YES) break;
    pointerExtEnt = pointerExtEnt->next;
  }
  
  pointerExtEnt = (*headExtEnt);
  
  /* Write external and entry symbols to respective files */
  while(pointerExtEnt) {
    if(pointerExtEnt->type == EXTERNAL) fprintf(ext, "%-8s %04d\n", pointerExtEnt->varName, pointerExtEnt->lineNum);
    if(pointerExtEnt->type == ENTRY) fprintf(ent, "%-8s %04d\n", pointerExtEnt->varName, pointerExtEnt->lineNum);
    pointerExtEnt = pointerExtEnt->next;
  }
  
  /* Free linked lists and close files*/
  fclose(ob);
  fclose(ext);
  fclose(ent);
  free(fileNameExt);
  freeDataImg(headDataImg);
  freeCodeImg(headCodeImg);
  freeExtEnt(headExtEnt);
}

/******************************************************
 * Function: encrypt
 * Description: Converts binary strings to encrypted characters.
 * 
 * @param input: The binary string to encrypt.
 * @return The encrypted character representation of the binary string.
 ******************************************************/
char* encrypt(char* input) {
  if(strcmp(input, "00") == 0) return "*";
  else if(strcmp(input, "01") == 0) return "#";
  else if(strcmp(input, "10") == 0) return "%";
  else if(strcmp(input, "11") == 0) return "!";
  else return NULL;
}

/******************************************************
 * Function: freeDataImg
 * Description: Frees memory allocated for data image nodes and their content.
 * 
 * @param hptr: Pointer to the head of the data image linked list.
 ******************************************************/
void freeDataImg(ptrDataImg * hptr) {
  ptrDataImg p;
  
  /* Parse the linked list and free each node and its content */
  while(*hptr) { 
    p = *hptr;
    *hptr = (*hptr)->next;
    free(p->output); /* Free the output string */
    free(p); /* Free the node */
  }
}

/******************************************************
 * Function: freeCodeImg
 * Description: Frees memory allocated for code image nodes and their content.
 * 
 * @param hptr: Pointer to the head of the code image linked list.
 ******************************************************/
void freeCodeImg(ptrCodeImg * hptr) {
  ptrCodeImg p;
  
  /* Parse the linked list and free each node and its content */
  while(*hptr) { 
    p = *hptr;
    *hptr = (*hptr)->next; 
    free(p->output); /* Free the output string */
    free(p); /* Free the node */
  }
}

/******************************************************
 * Function: freeExtEnt
 * Description: Frees memory allocated for external/entry symbol nodes and their content.
 * 
 * @param hptr: Pointer to the head of the external/entry symbol linked list.
 ******************************************************/
void freeExtEnt(ptrExtEnt * hptr) {
  ptrExtEnt p;
  
  /* Parse the linked list and free each node and its content */
  while(*hptr) { 
    p = *hptr;
    *hptr = (*hptr)->next;
    free(p->varName); /* Free the variable name string */
    free(p); /* Free the node */
  }
}

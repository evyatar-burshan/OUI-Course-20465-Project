#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFFER 82 /*Maximum line length (plus one \n charcter and null terminator)*/
#define MAX_WORD 15 /*Maximum word length (plus one null terminator character*/
#define MAX_LABEL 32 /*Maximum label length (plus one null terminator character*/
#define MAX_PROGRAM 4096 /*Maximum program length*/
#define OPCODE 16 /*Number of opcodes*/

#define NO 0
#define YES 1

#define A 00 /*Absolute*/
#define R 10 /*Relocatable*/
#define E 01 /*External*/

#define DEFINE 0
#define CODE 1
#define DATA 2
#define EXTERNAL 3
#define ENTRY 4

#define IMMEDIATE 0
#define DIRECT 1
#define INDEX 2
#define DIRECT_REGISTER 3

typedef struct nodeDataImg* ptrDataImg;
typedef struct nodeDataImg {
  int lineNum;
  int L;
  char* output;
  ptrDataImg next;
} itemDataImg;

typedef struct nodeCodeImg* ptrCodeImg;
typedef struct nodeCodeImg {
  int lineNum;
  int opcode;
  int L;
  char* output;
  ptrCodeImg next;
} itemCodeImg;

typedef struct nodeExtEnt* ptrExtEnt;
typedef struct nodeExtEnt {
  int lineNum;
  int type;
  char* varName;
  ptrExtEnt next;
} itemExtEnt;

typedef struct nodeLabel* ptrLabel;
typedef struct nodeLabel {
  char* labelName;
  int labelType;
  int data;
  ptrLabel next;
} itemLabel;

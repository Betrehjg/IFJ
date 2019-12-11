//
// Autor: Malínek Libor, Listopad 2019
//

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

typedef enum{
    ndVariable, ndFunction
} tNodeType; //jestli promenna nebo funkce

//uzel
typedef struct tBSTNode {
    char *Key; //name?
    void  *BSTNodeCont;
    struct tBSTNode *LPtr;
    struct tBSTNode *RPtr;
    tNodeType nodeType; //typ uzlu
} *tBSTNodePtr;

void BSTInit(tBSTNodePtr *);
tBSTNodePtr BSTSearch(tBSTNodePtr, char *);
int BSTInsert(tBSTNodePtr *, char *, void *, tNodeType);
void BSTDelete(tBSTNodePtr *, char *);
void BSTDispose(tBSTNodePtr *);

typedef  struct variable{ //Content
    void *argContent;
    char *name; 
} inVariable; 

typedef  struct  function{ //content
    bool defined;
    int argContent;
} inFunction;

typedef struct symtable{
    tBSTNodePtr root;
} tSymtable;

void symtableInit(tSymtable *);
int symtableInsertVariable(tSymtable *, char *, void *);
int symtableInsertFunction(tSymtable *, char *, bool, int);
void symtableDelete(tSymtable *,char *);
void symtableDispose(tSymtable *);
tBSTNodePtr symtableSearch(tSymtable *, char *);

#endif //SYMTABLE_H

//
// Created by Matej Dubec on 23.11.2019.
//

#ifndef PROJEKT_STACK_H
#define PROJEKT_STACK_H

#include "token.h"
#include "psa.h"

typedef struct exprItem {      /// struktura pre polozku zasobnika vyrazu
    int is_terminal;
    PSA_operations_enum action;
    PSA_table_index_enum index;
    PSA_symbol_enum table_symbol;
    char *data;
    t_token src_token; //ID, hodnota
    char *var_name; //nazev promene ve ktere je ta vec
    int data_size;          //pocet znakov tokenu
    struct exprItem *next;                // dalsia polozka
} eItem;

typedef struct {               // struktura pre zasobnik
    struct exprItem *top;                 // ukazatel na vrchol zásobníka
} eStack;


/// STRUKTURA STACK / IND STACK A OPERACIE NAD NOU !!!

typedef struct item {        // struktura pre polozku zasobnika
    t_token *token;          // ukazatetel na t_token
    struct item *next;       // dalsia polozka
} TSItem;

typedef struct {             // struktura pre zasobnik
    TSItem *top;             // ukazatel na vrchol zásobníka
} stack;

typedef struct ind_item {    // struktura pre polozku zasobnika
    int indent;              // pocet medzier v tokene
    struct ind_item *next;   // dalsia polozka
} TSIndItem;

typedef struct {             // struktura pre zasobnik
    TSIndItem *top;          // ukazatel na vrchol zásobníka
} ind_stack;

/// Ak vrchol stacku ukazuje na NULL -> prazdny stack -> vraciam true
#define isEmpty(S) ((S)->top == NULL) ? (1) : (0)

void stackInit (stack *s);
int stackPush (stack *s, t_token current_token);
int stackPop (stack *s);
void *stackTop (stack *s);
void *stackTopPop (stack *s);
void stackDelete (stack *s);
void indentStackInit (ind_stack *s);
int indentStackPush (ind_stack *s, int indentation);
int indentStackPop (ind_stack *s);
void *indentStackTop (ind_stack *s);
void *indentStackTopPop (ind_stack *s);
void indentStackDelete (ind_stack *s);

void eStackInit (eStack *s);
int eStackPush (eStack *s, t_token * current_token, PSA_table_index_enum index, PSA_symbol_enum tableSymbol, int isT);
int eItemStackPush (eStack *s, eItem *nextTop);
int eStackPop (eStack *s);
void *eStackTop (eStack *s);
eItem *eStackTopItem (eStack *s);
eItem *eStackTopPop (eStack *s);
void eStackDelete (eStack *s);

#endif //PROJEKT_STACK_H
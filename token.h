//
// Created by Matej Dubec on 23.11.2019.
//

#ifndef PROJEKT_TOKEN_H
#define PROJEKT_TOKEN_H


#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

/// VYCET STAVOV V KONECNOM AUTOMATE

typedef enum {
    // koncove stavy
            ID, STR, DOCSTR, INTEGER, DOUBLE, LOG_EQ, LOG_NONEQ, LOG_MORE, LOG_LESS, LOG_LEQ, LOG_MEQ, LOG_NEQ,
    ADD, SUBSTRACT, MULTIPLY, DIVIDE, IDIVIDE, EOL, L_BRAC, R_BRAC, ASSIGN, COMMA, LEX_EOF, COLON,  //LEX_ERR

    // klucove stavy
            KW_DEF, KW_ELSE, KW_IF, KW_NONE, KW_PASS, KW_RETURN, KW_WHILE,

    // nekoncove stavy
            START, LINE_COMM, DOCSTR_INPUT, DOCSTR_BEGIN, DOCSTR_END, STR_INPUT, STR_BCKSLSH, DOCSTR_BCKSLSH, STR_HEX, STR_HEX_2, EXPR_T, EXPR_S, DOT, EXCLAIM,

    // odsadenie
            INDENT, DEDENT,

} t_state;

// TYPY TOKENU
static const char *token_state_string[] = {"ID", "STR", "DOCSTRING", "INTEGER", "DOUBLE", "LOG_EQ", "LOG_NONEQ", "LOG_MORE", "LOG_LESS,",
                                           "LOG_LEQ", "LOG_MEQ", "LOG_NEQ", "ADD", "SUBSTRACT", "MULTIPLY", "DIVIDE", "IDIVIDE", "EOL", "L_BRAC", "R_BRAC",
                                           "ASSIGN", "COMMA", "LEX_EOF", "COLON", "KW_DEF", "KW_ELSE", "KW_IF", "KW_NONE", "KW_PASS", "KW_RETURN",
                                           "KW_WHILE", "START", "LINE_COMM", "DOCSTR_INPUT", "DOCSTR_BEGIN", "DOCSR_END", "STR_INPUT", "STR_BCKLSG",
                                           "DOCSTR_BCKSLSH", "STR_HEX", "STR_HEX2", "EXPR_T", "EXPR_S", "DOT", "EXCLAIM", "INDENT", "DEDENT"};


/// STRUKTURA TOKENU

typedef struct {
    t_state type;
    char *data; // V pripade INDENT a DEDENT pocet medzier / inac znakova reprezentacia tokenu
    int data_size;
} t_token;

void init_token (t_token *current_token);
int add_char_token (t_token *current_token, int a);
int add_indent (t_token *current_token, int a);
void return_token (t_state current_state, t_token *current_token, int a, FILE *file, int *empty_line);
#endif //PROJEKT_TOKEN_H

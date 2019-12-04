//
// Created by Matej Dubec on 26.11.2019.
//
#include "token.h"

void init_token (t_token *current_token) {
    current_token->type = START;
    current_token->data = "";
    current_token->data_size = 0;
}

int add_char_token (t_token *current_token, int a) { //prida znak do tokenu, alokuje pamat, ret: 1=ERR, 0=OK
    if (current_token->data_size == 0) {
        if ((current_token->data = malloc(10* sizeof(int)+1)) == NULL) return INTERNAL_ERROR;
    }
    else if (current_token->data_size % 10 == 0) {
        if ((current_token->data = realloc(current_token->data, sizeof(char) * current_token->data_size + 10)) == NULL) return INTERNAL_ERROR;
    }
    current_token->data[current_token->data_size++] = (char) a;
    return OK;
}

int add_indent (t_token *current_token, int a) {
    if (current_token->data_size == 0) {
        if ((current_token->data = malloc(10* sizeof(int)+1)) == NULL) return INTERNAL_ERROR;
        current_token->data_size++;
    }

    current_token->data[0] = (char) a; // upravim info o pocte medzier
}


void return_token (t_state current_state, t_token *current_token, int a, FILE *file, int *empty_line) { //nastavi token
    ungetc(a, file);
    add_char_token(current_token, '\0');
    current_token->data_size--;
    current_token->type = current_state;
    *empty_line = 0;
}

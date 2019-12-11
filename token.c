//
// Created by Matej Dubec on 26.11.2019.
//
#include "token.h"

/// OPERACIE NAD STRUKTUROU TOKEN
void init_token (t_token *current_token) { // Inicializacia struktury reprezentujucej token
    current_token->type = START;
    current_token->data = "";
    current_token->data_size = 0;
}

int add_char_token (t_token *current_token, int a) { // Pridam znak do tokenu, alokujem pamat, 1 = ERROR, 0 = OK
    if (current_token->data_size == 0) { // Alokacia pamate
        if ((current_token->data = malloc(10* sizeof(int)+1)) == NULL) return INTERNAL_ERROR;
    }
    else if (current_token->data_size % 10 == 0) { // Potrebujem naalokovat viac
        if ((current_token->data = realloc(current_token->data, sizeof(char) * current_token->data_size + 10)) == NULL) return INTERNAL_ERROR;
    }
    current_token->data[current_token->data_size++] = (char) a;
    return OK;
}

int add_indent (t_token *current_token, int a) { // Pridavanie informacie o indentacii
    if (current_token->data_size == 0) {
        if ((current_token->data = malloc(10* sizeof(int)+1)) == NULL) return INTERNAL_ERROR;
        current_token->data_size++;
    }

    current_token->data[0] = (char) a; // Upravim info o pocte medzier
}


void return_token (t_state current_state, t_token *current_token, int a, FILE *file, int *empty_line) { // Vratim token parsru, vratim posledny nacitany znak
    ungetc(a, file);
    add_char_token(current_token, '\0');
    current_token->data_size--;
    current_token->type = current_state;
    *empty_line = 0;
}

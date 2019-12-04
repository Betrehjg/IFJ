//
// Created by Matej Dubec on 26.11.2019.
//

#ifndef PROJEKT_SCANNER_H
#define PROJEKT_SCANNER_H

#include "token.h"
#include "stack.h"

t_state token_keyword ( char *str);
int get_token(FILE *buffer, t_token *current_token, ind_stack *indentStack, int *newline);

#endif //PROJEKT_SCANNER_H

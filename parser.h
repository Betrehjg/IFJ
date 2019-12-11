

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "token.h"
#include "errors.h"
#include "stack.h"
#include "symtable.h"

struct shared_vars {
    t_token *n_token, *c_token;
    ind_stack *indet_stack;
    int ret_value;
    int new_line;      // flag na zistenie, ci mam pocitat medzeri do indentacie
    tSymtable glob_symtable;
    tSymtable loc_symtable;
    int undef_cnt;
    FILE *file;
}shared_vars;

int prog();
int func_def();
int stat();
int stat_list();
int stat_2(bool local, char *name);
int assign(bool local, t_token *src_token);
int assign_value(bool local, char *name, t_token *src_token);
int func_arg(bool local, int *argc, stack *args_stack);
int func_arg_next(bool local, int *argc, stack *args_stack);
int value(bool local, t_token *src_token);
int arg_def(int *argc);
int arg_def_next(int *argc);
int func_stat_list();
int func_stat();
int sp_expr(bool local, t_token *src_token);
int return_value();

bool is_expr();
int p_next_token();

int sem_func_test(char *name, bool defined, int argc);
int sem_var_test(char *name, bool local, void *content, bool define);

#define GET_TOKEN() \
do { \
    if ((shared_vars.ret_value = p_next_token()) != OK) \
         return shared_vars.ret_value; \
} while (0)

#define CHECK_KEYWORD(keyword) \
do { \
    if (shared_vars.c_token->type != keyword) \
        return SYN_ERROR; \
    GET_TOKEN(); \
} while (0)

#define  CHECK_RULE(rule) \
do { \
    if ((shared_vars.ret_value = rule()) != OK) \
        return shared_vars.ret_value; \
} while (0)

#define  CHECK_RULE_WARGS(rule) \
do { \
    if ((shared_vars.ret_value = rule) != OK) \
        return shared_vars.ret_value; \
} while (0)

#define COPY_TOKEN_DATA(dest) \
    char *dest = malloc(sizeof(char) * shared_vars.c_token->data_size); \
    if (dest == NULL) \
        return INTERNAL_ERROR; \
    strcpy(dest, shared_vars.c_token->data)

#define COPY_TOKEN(dest) \
    if (dest != NULL) {\
        dest->type =shared_vars.c_token->type; \
        if ((dest->data = malloc(sizeof(char) * shared_vars.c_token->data_size)) == NULL) \
            return INTERNAL_ERROR; \
        strcpy(dest->data, shared_vars.c_token->data); \
        dest->data_size = shared_vars.c_token->data_size; \
    }

#define IS_OP(token) (shared_vars.token->type == LOG_EQ || shared_vars.token->type == LOG_NONEQ || shared_vars.token->type == LOG_MORE || shared_vars.token->type == LOG_LESS || shared_vars.token->type == LOG_LEQ || shared_vars.token->type == LOG_NEQ \
 || shared_vars.token->type == ADD || shared_vars.token->type == SUBSTRACT || shared_vars.token->type == MULTIPLY || shared_vars.token->type == DIVIDE || shared_vars.token->type == IDIVIDE)

#define IS_TERM(token) (shared_vars.token->type == ID || IS_VALUE(token))

#define IS_VALUE(token) (shared_vars.token->type == STR || shared_vars.token->type == INTEGER || shared_vars.token->type == DOUBLE || shared_vars.token->type == DOCSTR || shared_vars.token->type == KW_NONE)

#endif //PARSER_H
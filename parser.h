/**
 * @file: parser.h
 * @author: Michal Zobaník (xzoban01)
 *
 * Hlavickovy soubor pro parser prekladace
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "token.h"
#include "errors.h"
#include "stack.h"
#include "symtable.h"

/**
 * Struktura pro prekladac
 */
struct shared_vars {
    t_token *n_token, *c_token; //aktualni a nasledujici token
    ind_stack *indet_stack; //pomocny stack pro scanner
    int ret_value; //pomocna promena pro navratovou hodnotu
    int new_line; //novy radek
    tSymtable glob_symtable; //global tabulka symbolu
    tSymtable loc_symtable; //lokalni tabulka symbolu
    int undef_cnt; //pocet nedefinovanych funkci
    FILE *file; //soubor ze ktereho se cte vstup
}shared_vars;

/**
 * kontroluje pravidla prog ->
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int prog();

/**
 * kontroluje pravidla func_def ->
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int func_def();

/**
 * kontroluje pravidla stat ->
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int stat();

/**
 * kontroluje pravidla stat_list ->
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int stat_list();

/**
 * kontroluje pravidla prog ->
 * @param local jsem ve funkci -> true, jinak false
 * @param name jmeno promene do ktere prirazuju / funkce kterou volam
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int stat_2(bool local, char *name);

/**
 * kontroluje pravidla assign ->
 * @param local jsem ve funkci -> true, jinak false
 * @param src_token token s daty o zdroji, ze ktereho budu prirazovat
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int assign(bool local, t_token *src_token);

/**
 * kontroluje pravidla assign_value ->
 * @param local jsem ve funkci -> true, jinak false
 * @param name jmeno promene do kterou prirazuji / funkce kterou volam
 * @param src_token token s daty kde je navratova hodnota funkce/promena
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int assign_value(bool local, char *name, t_token *src_token);

/**
 * kontroluje pravidla fuc_arg ->
 * @param local jsem ve funkci -> true, jinak false
 * @param argc pocet argumetnu funkce
 * @param args_stack stack s tokeny, ktere obsahuji informace o argumentech
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int func_arg(bool local, int *argc, stack *args_stack);

/**
 * kontroluje pravidla fuc_arg_next ->
 * @param local jsem ve funkci -> true, jinak false
 * @param argc pocet argumetnu funkce
 * @param args_stack stack s tokeny, ktere obsahuji informace o argumentech
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int func_arg_next(bool local, int *argc, stack *args_stack);

/**
 * kontroluje pravidla value ->
 * @param local jsem ve funkci -> true, jinak false
 * @param src_token token s daty ktery se budou prirazovat
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int value(bool local, t_token *src_token);

/**
 * kontroluje pravidla arg_def ->
 * @param argc pocet argumentu funkce
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int arg_def(int *argc);

/**
 * kontroluje pravidla arg_def_next ->
 * @param argc pocet argumentu funkce
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int arg_def_next(int *argc);

/**
 * kontroluje pravidla fuc_arg ->
 * @param func_name jmeno funkce, ve ktere se nachazim
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int func_stat_list(char *func_name);

/**
 * kontroluje pravidla fuc_arg ->
 * @param func_name jmeno funkce, ve ktere se nachazim
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int func_stat(char *func_name);

/**
 * kontroluje pravidla sp_expr ->
 * @param local jsem ve funkci -> true, jinak false
 * @param src_token token s daty ktere budou prirazena
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int sp_expr(bool local, t_token *src_token);

/**
 * kontroluje pravidla return_value ->
 * @param func_name jmeno funkce ve ktere se nachazim
 * @return OK kdyz je pravidlo v poradku, jinak vraci kod chyby
 */
int return_value(char *func_name);

/**
 * Zjistuje jestli je aktualni token soucat vyrazu
 * @return true jestli je akt. token soucasti vyrazu jinak false
 */
bool is_expr();

/**
 * Zazada si o dalsi token z scanneru
 * @return OK, kdyz nedoslo k chybe, jinka kod chyby
 */
int p_next_token();

/**
 * semanticka kotrola funkce
 * @param name jmeno funkce
 * @param defined
 * @param argc pocet argumentu
 * @return OK, kdyz nedoslo k chybe a neni semanticka chyba , jinka kod chyby
 */
int sem_func_test(char *name, bool defined, int argc);

/**
 * semanticka kontrola promenne
 * @param name jmeno promenne
 * @param local true, kdyz se jedna o lokalni promenou
 * @param content obsah promene
 * @param define true kdyz se jedna o definici promene jinka false
 * @return OK, kdyz nedoslo k chybe a neni semanticka chyba , jinka kod chyby
 */
int sem_var_test(char *name, bool local, void *content, bool define);

/**
 * pozada o dalsi token, pri neuspechu vraci kod chyby
 */
#define GET_TOKEN() \
do { \
    if ((shared_vars.ret_value = p_next_token()) != OK) \
         return shared_vars.ret_value; \
} while (0)

/**
 * Zkontroluje ze akt. token ma spravny typ, jinka vraci SYN_ERROR, pozada o dalsi token
 */
#define CHECK_KEYWORD(keyword) \
do { \
    if (shared_vars.c_token->type != keyword) \
        return SYN_ERROR; \
    GET_TOKEN(); \
} while (0)

/**
 * Zkontoluje pravidlo rule, pri chybe vraci kod chyby
 */
#define  CHECK_RULE(rule) \
do { \
    if ((shared_vars.ret_value = rule()) != OK) \
        return shared_vars.ret_value; \
} while (0)

/**
 * Zkontoluje pravidlo rule, ktera prijima argumenty, pri chybe vraci kod chyby
 */
#define  CHECK_RULE_WARGS(rule) \
do { \
    if ((shared_vars.ret_value = rule) != OK) \
        return shared_vars.ret_value; \
} while (0)

/**
 * Zkopiruje atribut data aktualniho tokenu do dest
 */
#define COPY_TOKEN_DATA(dest) \
    char *dest = malloc(sizeof(char) * shared_vars.c_token->data_size); \
    if (dest == NULL) \
        return INTERNAL_ERROR; \
    strcpy(dest, shared_vars.c_token->data)

/**
 * Zkopiruje cely obsah akt. tokenu do dest
 */
#define COPY_TOKEN(dest) \
    if (dest != NULL) {\
        dest->type =shared_vars.c_token->type; \
        if ((dest->data = malloc(sizeof(char) * shared_vars.c_token->data_size)) == NULL) \
            return INTERNAL_ERROR; \
        strcpy(dest->data, shared_vars.c_token->data); \
        dest->data_size = shared_vars.c_token->data_size; \
    }

/**
 * kontrola ze token je operator
 */
#define IS_OP(token) (shared_vars.token->type == LOG_EQ || shared_vars.token->type == LOG_NONEQ || shared_vars.token->type == LOG_MORE || shared_vars.token->type == LOG_LESS || shared_vars.token->type == LOG_LEQ || shared_vars.token->type == LOG_NEQ \
 || shared_vars.token->type == ADD || shared_vars.token->type == SUBSTRACT || shared_vars.token->type == MULTIPLY || shared_vars.token->type == DIVIDE || shared_vars.token->type == IDIVIDE)

/**
* kontrola ze token je term
*/
#define IS_TERM(token) (shared_vars.token->type == ID || IS_VALUE(token))

/**
 * kontrola ze token je hodnota
 */
#define IS_VALUE(token) (shared_vars.token->type == STR || shared_vars.token->type == INTEGER || shared_vars.token->type == DOUBLE || shared_vars.token->type == DOCSTR || shared_vars.token->type == KW_NONE)

#endif //PARSER_H
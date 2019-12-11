/**
 * @file: code_gen.h
 * @authors: Matej Dubec, Samuel Spišák, Michal Zobaník, Libor Malínek
 *
 * hlavickovy soubor s funkcemi pro generovani IFJcode
 */

#ifndef PARSER_CODE_GEN_H
#define PARSER_CODE_GEN_H

#include "token.h"
#include "stack.h"

int gen_prog_start();
void gen_def_var (char *name_var, bool local);
void gen_print_decl ();
void gen_inputf_decl (char *input_type);
void gen_len_decl ();
void gen_chr_decl ();
void gen_ord_decl ();
void gen_substr_decl ();
int gen_assign(char *dest_name, t_token src_token, bool local);
int gen_type_control(t_state type1, char *cont1, t_state type2, char *cont2, bool local);
int gen_func_start(char *func_name);
int gen_func_end(char *func_name);
int gen_else(int idx);
int gen_if_end(int idx);
int gen_func_arg(char *name);
int gen_func_call(char *name, int argc, stack *args_stack, bool local);
int gen_return(char *func_name, t_token src_token, bool local);
char *create_var(t_state type, char *cont, bool local);
int gen_aritm_op(PSA_rules_enum type, char *op1, char *op2, char *dest, bool local);
int gen_if_start(char *name, bool local, int *counter);




#endif //PARSER_CODE_GEN_H

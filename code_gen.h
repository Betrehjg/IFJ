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

/**
 * vygenerovani hlavicky, vestavenych funkci
 * @return chybovy kod, nebo OK
 */
int gen_prog_start();

/**
 * generovani deklarace promene
 * @param name_var
 * @param local true -> je ve funkci jinak false
 */
void gen_def_var (char *name_var, bool local);

/**
 * generovani print()
 */
void gen_print_decl ();

/**
 * generovai inputi/s/f()
 * @param input_type typ vstupu
 */
void gen_inputf_decl (char *input_type);

/**
 * generovani len()
 */
void gen_len_decl ();

/**
 * generovani chr()
 */
void gen_chr_decl ();

/**
 * generovani ord()
 */
void gen_ord_decl ();

/**
 * generovani substr()
 */
void gen_substr_decl ();

/**
 * generovani prirazeni
 *
 * @param dest_name jmeno cile
 * @param src_token token s udaji o zdroji
 * @param local true -> je ve funkci jinak false
 * @return chybovy kod, nebo OK
 */
int gen_assign(char *dest_name, t_token src_token, bool local);

/**
 * generovani typove kontroly/pretypovani
 *
 * @param type1 typ prvni hodnoty
 * @param cont1 obsah prvni hodnoty
 * @param type2 typ druhe hodnoty
 * @param cont2 obsah druhe hodnoty
 * @param local true -> je ve funkci jinak false
 * @return chybovy kod, nebo OK
 */
int gen_type_control(t_state type1, char *cont1, t_state type2, char *cont2, bool local);

/**
 * Generovani zacatku definice funkce
 *
 * @param func_name jmeno funkce
 * @return chybovy kod, nebo OK
 */
int gen_func_start(char *func_name);

/**
 * Generovani konce definice funkce
 *
 * @param func_name jmeno funkce
 * @return chybovy kod, nebo OK
 */
int gen_func_end(char *func_name);

/**
 * Generovani zacatku if
 *
 * @param name jmeno promene ve ktere je vyhodnoceni podminky
 * @param local true -> je ve funkci jinak false
 * @param counter pocitadlo pro unikatni navesti
 * @return chybovy kod, nebo OK
 */
int gen_if_start(char *name, bool local, int *counter);

/**
 * Generuje else vetev if
 *
 * @param idx pocitadlo pro unikatni navesti
 * @return chybovy kod, nebo OK
 */
int gen_else(int idx);

/**
 * Generuje konec if
 *
 * @param idx pocitadlo pro unikatni navesti
 * @return chybovy kod, nebo OK
 */
int gen_if_end(int idx);

/**
 * Generuje kod pro ziskani argumentu funkce ze zasobniku
 *
 * @param name jmeno argumentu
 * @return chybovy kod, nebo OK
 */
int gen_func_arg(char *name);

/**
 * Generuje volani funkce
 *
 * @param name jmeno funkce
 * @param argc pocet argumentu
 * @param args_stack stack s argumenty
 * @param local true -> je ve funkci jinak false
 * @return chybovy kod, nebo OK
 */
int gen_func_call(char *name, int argc, stack *args_stack, bool local);

/**
 * Generovani return ve funkci
 *
 * @param func_name jmeno funkce
 * @param src_token token s daty o hodnote ktera se ma vratit ve funkci
 * @param local true -> je ve funkci jinak false
 * @return chybovy kod, nebo OK
 */
int gen_return(char *func_name, t_token src_token, bool local);

/**
 * Prida k jmenu promene/konstanty ramec ve ktere promena je
 *
 * @param type typ hodnoty
 * @param cont obsah
 * @param local true -> je ve funkci jinak false
 * @return Nazev promene/konstanty s ramcem ve ktere se nachazi
 */
char *create_var(t_state type, char *cont, bool local);

/**
 * Generovani aritmetickych operaci
 *
 * @param type druh operace
 * @param op1 jmeno 1. operatoru
 * @param op2 jmeno 2. operatoru
 * @param dest jmeno cilova promene
 * @param local true -> je ve funkci jinak false
 * @return chybovy kod, nebo OK
 */
int gen_aritm_op(PSA_rules_enum type, char *op1, char *op2, char *dest, bool local);

#endif //PARSER_CODE_GEN_H

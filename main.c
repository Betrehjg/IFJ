#include <stdio.h>
#include "parser.h"
#include "stack.h"
#include "symtable.h"
#include "code_gen.h"

//#define DEBUG

int main() {
    //test
    shared_vars.file = fopen("code_gen1", "r");

    //alokace pameti
    shared_vars.c_token = malloc(sizeof(t_token));
    shared_vars.n_token = malloc(sizeof(t_token));
    shared_vars.indet_stack = malloc(sizeof(ind_stack));

    //inicializace globalnich promenych
    init_token(shared_vars.c_token);
    init_token(shared_vars.n_token);
    indentStackInit(shared_vars.indet_stack);
    shared_vars.new_line = 1;
    shared_vars.undef_cnt = 0;

    //inicializace glob. tab. symb.
    symtableInit(&shared_vars.glob_symtable);

    //vložení vestvěných funkcí do tabulky
    symtableInsertFunction(&shared_vars.glob_symtable, "inputs", true, 0);
    symtableInsertFunction(&shared_vars.glob_symtable, "inputf", true, 0);
    symtableInsertFunction(&shared_vars.glob_symtable, "inputi", true, 0);
    symtableInsertFunction(&shared_vars.glob_symtable, "print", true, -1); //libovolný počet ?
    symtableInsertFunction(&shared_vars.glob_symtable, "len", true, 1);
    symtableInsertFunction(&shared_vars.glob_symtable, "substr", true, 3);
    symtableInsertFunction(&shared_vars.glob_symtable, "ord", true, 2);
    symtableInsertFunction(&shared_vars.glob_symtable, "chr", true, 1);

    //generovani hlavicky a vestavenych funkci
    gen_prog_start();

    #ifdef DEBUG
    do {
        D_GET_TOKEN();
    } while (shared_vars.c_token->type != LEX_EOF);

    return 0;
    #else
    GET_TOKEN();

    if ((shared_vars.ret_value = prog()) != OK)
        return shared_vars.ret_value;

    if (shared_vars.undef_cnt != 0)
        return SEM_ERROR;

        return 0;
    #endif
}
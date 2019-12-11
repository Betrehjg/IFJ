#include "parser.h"
#include "scanner.h"
#include "psa.h"
#include "code_gen.h"
#include "stack.h"
#include <string.h>

int prog() {

    //<prog> -> <stat_list> <func_def>
    if (is_expr() || shared_vars.c_token->type == KW_IF || shared_vars.c_token->type == ID || shared_vars.c_token->type == KW_WHILE || shared_vars.c_token->type == KW_PASS || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        CHECK_RULE(stat_list);

        return func_def();
    } //<prog> -> <func_def>
    else if (shared_vars.c_token->type == KW_DEF || shared_vars.c_token->type == LEX_EOF) {
        return func_def();
    }  else {
        return SYN_ERROR;
    }
}

int func_def() {
    //<func_def> -> DEF ID ( <arg_def> ) : EOL INDENT <func_stat_list> DEDENT <prog>
    if (shared_vars.c_token->type == KW_DEF) {
        //Vytvoreni lokalni tabulky pro funkci
        symtableInit(&shared_vars.loc_symtable);

        GET_TOKEN();
        //ulozeni contentu nasledujiciho tokenu jako jmeno funkce
        COPY_TOKEN_DATA(name);

        //TODO: generovani zacatku fuknce, (label pro preskoceni)
        gen_func_start(name);

        CHECK_KEYWORD(ID);
        CHECK_KEYWORD(L_BRAC);

        int argc = 0; //pocet argumentu funkce
        CHECK_RULE_WARGS(arg_def(&argc));

        //kontrola id funkce
        if ((shared_vars.ret_value = sem_func_test(name, true, argc)) != OK)
            return shared_vars.ret_value;

        CHECK_KEYWORD(R_BRAC);
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(func_stat_list);

        CHECK_KEYWORD(DEDENT);

        gen_func_end(name);

        //odstraneni lokalni tabulky pro funkci
        symtableDispose(&shared_vars.loc_symtable);
        free(name);
        return prog();
    } //<func_def> -> eps
    else if (shared_vars.c_token->type == LEX_EOF) {
        return OK;
    }

    return SYN_ERROR;
}

int stat_list() {
    //<stat_list> -> <sp_expr> EOL <stat>
    if (is_expr() || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        //nic negeneruju
        CHECK_RULE_WARGS(sp_expr(false, NULL));

        CHECK_KEYWORD(EOL);

        return stat();
    } //<stat_list> -> ID <stat_2> EOL <stat>
    if (shared_vars.c_token->type == ID) {
        COPY_TOKEN_DATA(name);
        GET_TOKEN();

        CHECK_RULE_WARGS(stat_2(false, name));
        CHECK_KEYWORD(EOL);
        return stat();
    } //<stat_list> -> IF <sp_expr> : EOL INDENT <stat_list> DEDENT ELSE : EOL INDENT <stat_list> DEDENT  <stat>
    else if (shared_vars.c_token->type == KW_IF) {
        GET_TOKEN();

        t_token src_token;
        CHECK_RULE_WARGS(sp_expr(false, &src_token));

        //TODO: generovani zacatku if, vyhodnoceni podminky
        gen_if_start();

        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(stat_list);

        CHECK_KEYWORD(DEDENT);
        CHECK_KEYWORD(KW_ELSE);
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        //TODO: generovani konce if(true), generovani else vetve
        gen_else();

        CHECK_RULE(stat_list);

        CHECK_KEYWORD(DEDENT);

        //TODO: generovani konce if
        gen_if_end();

        return stat();
    } //<stat_list> -> WHILE <sp_expr> : EOL INDENT <stat_list> DEDENT <stat>
    else if (shared_vars.c_token->type == KW_WHILE) {
        GET_TOKEN();

        //TODO: generovani zacatku while
        t_token src_token;
        CHECK_RULE_WARGS(sp_expr(false, &src_token));

        //TODO: generovani podminky

        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(stat_list);

        CHECK_KEYWORD(DEDENT);

        //TODO generovani konce while

        return stat();
    } //<stat_list> -> PASS EOL <stat>
    else if (shared_vars.c_token->type == KW_PASS) {
        GET_TOKEN();
        CHECK_KEYWORD(EOL);

        return stat();
    }
    return SYN_ERROR;
}

int arg_def(int *argc) {
    //<arg_def> -> ID <arg_def_next>
    if (shared_vars.c_token->type == ID) {
        (*argc)++;
        COPY_TOKEN_DATA(name);

        if ((shared_vars.ret_value = sem_var_test(name, true, NULL, true)) != OK)
            return shared_vars.ret_value;

        gen_func_arg(name);

        GET_TOKEN();
        return arg_def_next(argc);
    } //<arg_def> -> eps
    else if (shared_vars.c_token->type == R_BRAC) {
        return OK;
    }

    return SYN_ERROR;
}

int func_stat_list() {
    //<func_stat_list> -> <sp_expr> EOL <func_stat>
    if (is_expr() || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        CHECK_RULE_WARGS(sp_expr(true, NULL));
        CHECK_KEYWORD(EOL);

        return func_stat();

    } //<func_stat_list> -> ID <stat_2> EOL <func_stat>
    else if (shared_vars.c_token->type == ID) {
        COPY_TOKEN_DATA(name);
        GET_TOKEN();

        CHECK_RULE_WARGS(stat_2(true, name));

        CHECK_KEYWORD(EOL);

        return  func_stat();
    } //<func_stat_list> -> IF <sp_expr> : EOL INDENT <func_stat_list> DEDENT ELSE : EOL INDENT <func_stat_list> DEDENT <func_stat>
    else if (shared_vars.c_token->type == KW_IF) {
        GET_TOKEN();

        t_token src_token;
        CHECK_RULE_WARGS(sp_expr(true, &src_token));

        //TODO: generovani zacatku if a kontola podminky

        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(func_stat_list);
        CHECK_KEYWORD(DEDENT);
        CHECK_KEYWORD(KW_ELSE);
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        //TODO: generovani konce if(true) a zacatek else

        CHECK_RULE(func_stat_list);
        CHECK_KEYWORD(DEDENT);

        //TODO: generovani konce if
        return func_stat();
    } //<func_stat_list> -> WHILE <sp_expr> : EOL INDENT <func_stat_list> DEDENT <func_stat>
    else if (shared_vars.c_token->type == KW_WHILE) {
        GET_TOKEN();
        //TODO: generovani zacatku while
        t_token src_token;
        CHECK_RULE_WARGS(sp_expr(true, &src_token));
        //TODO: kontrola podminky

        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(func_stat_list);
        CHECK_KEYWORD(DEDENT);

        //TODO: generovani smycky a  konce while
        return func_stat();
    } //<func_stat_list> -> PASS EOL <func_stat>
    else if (shared_vars.c_token->type == KW_PASS) {
        GET_TOKEN();
        CHECK_KEYWORD(EOL);

        return func_stat();
    } //<func_stat_list> -> RETURN <return_value> EOL <func_stat>
    else if (shared_vars.c_token->type == KW_RETURN) {
        GET_TOKEN();

        CHECK_RULE(return_value);
        CHECK_KEYWORD(EOL);

        return func_stat();
    }

    return SYN_ERROR;
}

int func_stat() {
    //<stat> -> <func_stat_list>
    if (is_expr() || shared_vars.c_token->type == ID || shared_vars.c_token->type == KW_IF || shared_vars.c_token->type == KW_WHILE || shared_vars.c_token->type == KW_PASS || shared_vars.c_token->type == KW_RETURN  || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        return func_stat_list();
    } //<func_stat> -> eps
    else if (shared_vars.c_token->type == DEDENT) {
        return OK;
    }

    return SYN_ERROR;
}

int sp_expr(bool local, t_token *src_token) {
    //<sp_expr> -> <expr>
    if (is_expr()) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        //TODO: ziskat src
        return expression(shared_vars.c_token, local);
    } //<sp_expr> -> <value>
    else if (IS_VALUE(c_token)) {
        return value(local, src_token);
    }

    return SYN_ERROR;
}

int assign(bool local, t_token *src_token) {
    //<asign> -> <sp_expr>
    if (is_expr() || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        return sp_expr(local, src_token);
    } //<asign> -> ID <asign_value>
    else if (shared_vars.c_token->type == ID) {
        COPY_TOKEN_DATA(src_name);
        GET_TOKEN();

        return assign_value(local, src_name, src_token);
    }

    return SYN_ERROR;
}

int stat() {
    //<stat> -> <stat_list>
    if (is_expr() || shared_vars.c_token->type == ID || shared_vars.c_token->type == KW_IF || shared_vars.c_token->type == KW_WHILE || shared_vars.c_token->type == KW_PASS || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        return stat_list();
    } //<stat> -> eps
    else if (shared_vars.c_token->type == KW_DEF || shared_vars.c_token->type == DEDENT || shared_vars.c_token->type == LEX_EOF) {
        return OK;
    }

    return SYN_ERROR;
}

int stat_2(bool local, char *name) {
    //<stat_2> -> = <asign>
    if (shared_vars.c_token->type == ASSIGN) {
        GET_TOKEN();

        //kontorla dest. ID
        if ((shared_vars.ret_value = sem_var_test(name, local, NULL, true)) != OK)
            return shared_vars.ret_value;

        t_token src_token;
        CHECK_RULE_WARGS(assign(local, &src_token));

        gen_assign(name, src_token, local);
        return OK;
    } //<stat_2> -> ( <func_arg> )
    else if (shared_vars.c_token->type == L_BRAC) {
        GET_TOKEN();

        int argc = 0;
        stack args_stack;
        stackInit(&args_stack);
        CHECK_RULE_WARGS(func_arg(local, &argc, &args_stack));

        //kontrola id funkce
        if ((shared_vars.ret_value = sem_func_test(name, false, argc)) != OK)
            return shared_vars.ret_value;

        gen_func_call(name, argc, &args_stack, local);
        stackDelete(&args_stack);
        GET_TOKEN();
        return OK;
    }

    return SYN_ERROR;
}

int return_value() {
    //<return_value> -> <asign>
    if (is_expr() || shared_vars.c_token->type == ID || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        t_token src_token;
        CHECK_RULE_WARGS(assign(true, &src_token));

        gen_assign("$ret_val", src_token, true);
        return OK;
    } //<return_value> -> eps
    else if (shared_vars.c_token->type == EOL) {
        t_token src_token;
        src_token.type = KW_NONE;
        gen_assign("$ret_val", src_token, true);

        return OK;
    }

    return SYN_ERROR;
}

int func_arg(bool local, int *argc, stack *args_stack) {
    //<func_arg> -> <asign> <func_arg_next>
    if (is_expr() || shared_vars.c_token->type == ID || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        t_token src_token;
        CHECK_RULE_WARGS(assign(local, &src_token));

        stackPush(args_stack, src_token);
        (*argc)++;
        return  func_arg_next(local, argc, args_stack);
    } //<func_arg> -> eps
    else if (shared_vars.c_token->type == R_BRAC) {
        return OK;
    }

    return SYN_ERROR;
}

int value(bool local, t_token *src_token) {
    //<value> -> INTEGER/STR/FLOAT/DOCSTR/NONE
    if (IS_VALUE(c_token)) {
        COPY_TOKEN(src_token);
        GET_TOKEN();
        return OK;
    }
    return SYN_ERROR;
}

int assign_value(bool local, char *name, t_token *src_token) {
    //<asign_value> -> ( <func_arg> )
    if (shared_vars.c_token->type == L_BRAC) {
        GET_TOKEN();

        int argc = 0;
        stack args_stack;
        stackInit(&args_stack);
        CHECK_RULE_WARGS(func_arg(local, &argc, &args_stack));
        CHECK_KEYWORD(R_BRAC);

        //kontrola id funkce
        if ((shared_vars.ret_value = sem_func_test(name, false, argc)) != OK)
            return shared_vars.ret_value;

        gen_func_call(name, argc, &args_stack, local);

        src_token->type = ID;
        src_token->data_size = strlen("$ret_val");
        src_token->data = "$ret_val";

        return OK;
    } //<asign_value> -> eps
    else if (shared_vars.c_token->type == R_BRAC || shared_vars.c_token->type == EOL || shared_vars.c_token->type == COMMA) {
        if ((shared_vars.ret_value = sem_var_test(name, local, NULL, false)) != OK)
            return shared_vars.ret_value;

        src_token->type = ID;
        src_token->data_size = strlen(name);
        src_token->data = name;

        return OK;
    }

    return SYN_ERROR;
}

int func_arg_next(bool local, int *argc, stack *args_stack) {
    //<func_arg_next> -> eps
    if (shared_vars.c_token->type == R_BRAC) {
        return OK;
    } //<func_arg_next> -> , <asign> <func_arg_next>
    else if (shared_vars.c_token->type == COMMA) {
        GET_TOKEN();
        t_token src_token;
        CHECK_RULE_WARGS(assign(local, &src_token));

        stackPush(args_stack, src_token);
        (*argc)++;

        return func_arg_next(local, argc, args_stack);
    }

    return SYN_ERROR;
}

int arg_def_next(int *argc) {
    //<arg_def_next> -> eps
    if (shared_vars.c_token->type == R_BRAC) {
        return OK;
    } //<arg_def_next> -> , ID <arg_def_next>
    else if(shared_vars.c_token->type == COMMA) {
        GET_TOKEN();

        COPY_TOKEN_DATA(name);
        CHECK_KEYWORD(ID);

        (*argc)++;
        if ((shared_vars.ret_value = sem_var_test(name, true, NULL, true)) != OK)
            return shared_vars.ret_value;

        gen_func_arg(name);

        return arg_def_next(argc);
    }

    return SYN_ERROR;
}

bool is_expr() {
    if (shared_vars.n_token->type == START) {
        if ((shared_vars.ret_value = get_token(shared_vars.file, shared_vars.n_token, shared_vars.indet_stack, &shared_vars.new_line)) != OK)
            return false;

        #ifdef DEBUG
            printf("%s - %s(%d)\n", token_state_string[shared_vars.n_token->type], shared_vars.n_token->data, shared_vars.n_token->data_size);
        #endif //DEBUG
    } else {
        shared_vars.ret_value = OK;
    }

    if ((IS_TERM(c_token) && IS_OP(n_token)) || (shared_vars.c_token->type == L_BRAC && (shared_vars.n_token->type == L_BRAC || IS_TERM(n_token))))
        return true;

    return false;
}

int p_next_token() {
    int ret;
    if (shared_vars.c_token->data_size != 0) //uvolneni pameti stareho tokenu
        free(shared_vars.c_token->data);

    if (shared_vars.n_token->type != START) {
        shared_vars.c_token->type = shared_vars.n_token->type;
        shared_vars.c_token->data_size = shared_vars.n_token->data_size;
        shared_vars.c_token->data = shared_vars.n_token->data;

        shared_vars.n_token->type = START;
    } else {
        ret = get_token(shared_vars.file, shared_vars.c_token, shared_vars.indet_stack, &shared_vars.new_line);

        #ifdef DEBUG
            printf("%s - %s(%d)\n", token_state_string[shared_vars.c_token->type], shared_vars.c_token->data, shared_vars.c_token->data_size);
        #endif //DEBUG
        return ret;
    }

    return OK;
}

int sem_func_test(char *name, bool defined, int argc) {
    tBSTNodePtr node = symtableSearch(&shared_vars.glob_symtable, name);

    if (node != NULL) {
        if (node->nodeType != ndFunction)
            return SEM_ERROR;

        inFunction *content = (inFunction*) node->BSTNodeCont;

        //kontrola poctu argumentu
        if (content->argContent != -1) {
            if (argc != content->argContent)
                return SEM_PCOUNT_ERROR;
        }

        //kontrola definice
        if (defined == true && content->defined == false) { //definovani nedefinovane
            if((shared_vars.ret_value = symtableInsertFunction(&shared_vars.glob_symtable, name, defined, argc)) != OK) \
                return shared_vars.ret_value;

            shared_vars.undef_cnt--;
        } else if (defined == true && content->defined == true) { //redefinovani funkce
            return SEM_ERROR;
        }

        return OK;
    } else {
        if (!defined)
            shared_vars.undef_cnt++;

        return symtableInsertFunction(&shared_vars.glob_symtable, name, defined, argc);
    }
}

int sem_var_test(char *name, bool local, void *content, bool define) {
    tBSTNodePtr node = symtableSearch(&shared_vars.glob_symtable, name);
    tBSTNodePtr nodeL = NULL;
    if (local)
        nodeL = symtableSearch(&shared_vars.loc_symtable, name);

    if (node != NULL || nodeL != NULL) {
        if (local) {
            //je definovana jako lokalni promena
            if (nodeL != NULL && nodeL->nodeType != ndVariable)
                return SEM_ERROR;
            else if (nodeL != NULL)
                return OK;
            else if (define == true) {
                shared_vars.ret_value = symtableInsertVariable(&shared_vars.loc_symtable, name, NULL);
                gen_def_var(name, local);
                return shared_vars.ret_value;
            }

            //neni lokalni, kontrola globalni
            if (node->nodeType != ndVariable)
                return SEM_ERROR;
            else
                return OK;
        } else {
            if (node->nodeType != ndVariable)
                return SEM_ERROR;
        }
    } else {
        if (define == false)
            return SEM_ERROR;

        if (local)
            shared_vars.ret_value = symtableInsertVariable(&shared_vars.loc_symtable, name, NULL);
        else
            shared_vars.ret_value = symtableInsertVariable(&shared_vars.glob_symtable, name, NULL);

        gen_def_var(name, local);

        return shared_vars.ret_value;
    }
}
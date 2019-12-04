#include "parser.h"
#include "scanner.h"
#include "psa.h"
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

        //odstraneni lokalni tabulky pro funkci
        symtableDispose(&shared_vars.loc_symtable);

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

        CHECK_RULE_WARGS(sp_expr(false));

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
        CHECK_RULE_WARGS(sp_expr(false));

        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(stat_list);

        CHECK_KEYWORD(DEDENT);
        CHECK_KEYWORD(KW_ELSE);
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(stat_list);

        CHECK_KEYWORD(DEDENT);

        return stat();
    } //<stat_list> -> WHILE <sp_expr> : EOL INDENT <stat_list> DEDENT <stat>
    else if (shared_vars.c_token->type == KW_WHILE) {
        GET_TOKEN();
        CHECK_RULE_WARGS(sp_expr(false));

        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(stat_list);

        CHECK_KEYWORD(DEDENT);

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

        CHECK_RULE_WARGS(sp_expr(true));
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

        CHECK_RULE_WARGS(sp_expr(true));
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(func_stat_list);
        CHECK_KEYWORD(DEDENT);
        CHECK_KEYWORD(KW_ELSE);
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(func_stat_list);
        CHECK_KEYWORD(DEDENT);

        return func_stat();
    } //<func_stat_list> -> WHILE <sp_expr> : EOL INDENT <func_stat_list> DEDENT <func_stat>
    else if (shared_vars.c_token->type == KW_WHILE) {
        GET_TOKEN();

        CHECK_RULE_WARGS(sp_expr(true));
        CHECK_KEYWORD(COLON);
        CHECK_KEYWORD(EOL);
        CHECK_KEYWORD(INDENT);

        CHECK_RULE(func_stat_list);
        CHECK_KEYWORD(DEDENT);

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

int sp_expr(bool local) {
    //<sp_expr> -> <expr>
    if (is_expr()) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        return expression(shared_vars.c_token, local);
    } //<sp_expr> -> <value>
    else if (IS_VALUE(c_token)) {
        return value();
    }

    return SYN_ERROR;
}

int assign(bool local) {
    //<asign> -> <sp_expr>
    if (is_expr() || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        return sp_expr(local);
    } //<asign> -> ID <asign_value>
    else if (shared_vars.c_token->type == ID) {
        COPY_TOKEN_DATA(name);
        GET_TOKEN();

        return assign_value(local, name);
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
    tBSTNodePtr node;

    //<stat_2> -> = <asign>
    if (shared_vars.c_token->type == ASSIGN) {
        GET_TOKEN();

        //kontorla dest. ID
        if ((shared_vars.ret_value = sem_var_test(name, local, NULL, true)) != OK)
            return shared_vars.ret_value;

        return assign(local);
    } //<stat_2> -> ( <func_arg> )
    else if (shared_vars.c_token->type == L_BRAC) {
        GET_TOKEN();

        int argc = 0;
        CHECK_RULE_WARGS(func_arg(local, &argc));

        //kontrola id funkce
        if ((shared_vars.ret_value = sem_func_test(name, false, argc)) != OK)
            return shared_vars.ret_value;

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

        return assign(true);
    } //<return_value> -> eps
    else if (shared_vars.c_token->type == EOL) {
        return OK;
    }

    return SYN_ERROR;
}

int func_arg(bool local, int *argc) {
    //<func_arg> -> <asign> <func_arg_next>
    if (is_expr() || shared_vars.c_token->type == ID || IS_VALUE(c_token)) {
        if (shared_vars.ret_value != OK) //kontrola chyby pri is_epr
            return shared_vars.ret_value;

        CHECK_RULE_WARGS(assign(local));
        (*argc)++;
        return  func_arg_next(local, argc);
    } //<func_arg> -> eps
    else if (shared_vars.c_token->type == R_BRAC) {
        return OK;
    }

    return SYN_ERROR;
}

int value() {
    //<value> -> INTEGER
    if (shared_vars.c_token->type == INTEGER) {
        GET_TOKEN();
        return OK;
    } //<value> -> DOUBLE
    else if (shared_vars.c_token->type == DOUBLE) {
        GET_TOKEN();
        return OK;
    } //<value> -> STRING
    else if (shared_vars.c_token->type == STR) {
        GET_TOKEN();
        return OK;
    }//<value> -> NONE
    else if (shared_vars.c_token->type == KW_NONE) {
        GET_TOKEN();
        return OK;
    } //<value> -> DOCSTR
    else if (shared_vars.c_token->type == DOCSTR) {
        GET_TOKEN();
        return OK;
    }

    return SYN_ERROR;
}

int assign_value(bool local, char *name) {
    //<asign_value> -> ( <func_arg> )
    if (shared_vars.c_token->type == L_BRAC) {
        GET_TOKEN();

        int argc = 0;
        CHECK_RULE_WARGS(func_arg(local, &argc));
        CHECK_KEYWORD(R_BRAC);

        //kontrola id funkce
        if ((shared_vars.ret_value = sem_func_test(name, false, argc)) != OK)
            return shared_vars.ret_value;

        return OK;
    } //<asign_value> -> eps
    else if (shared_vars.c_token->type == R_BRAC || shared_vars.c_token->type == EOL || shared_vars.c_token->type == COMMA) {
        if ((shared_vars.ret_value = sem_var_test(name, local, NULL, false)) != OK)
            return shared_vars.ret_value;

        return OK;
    }

    return SYN_ERROR;
}

int func_arg_next(bool local, int *argc) {
    //<func_arg_next> -> eps
    if (shared_vars.c_token->type == R_BRAC) {
        return OK;
    } //<func_arg_next> -> , <asign> <func_arg_next>
    else if (shared_vars.c_token->type == COMMA) {
        GET_TOKEN();
        CHECK_RULE_WARGS(assign(local));

        (*argc)++;

        return func_arg_next(local, argc);
    }

    return SYN_ERROR;
}

/*
int expr() {
    while (shared_vars.c_token->type != COLON && shared_vars.c_token->type != EOL) {
        GET_TOKEN();
    }
    return OK;
} */

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

        return arg_def_next(argc);
    }

    return SYN_ERROR;
}

bool is_expr() {
    if (shared_vars.n_token->type == START) {
        if ((shared_vars.ret_value = get_token(shared_vars.file, shared_vars.n_token, shared_vars.indet_stack, &shared_vars.new_line)) != OK)
            return false;

        printf("%s - %s(%d)\n", token_state_string[shared_vars.n_token->type], shared_vars.n_token->data, shared_vars.n_token->data_size);
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

        printf("%s - %s(%d)\n", token_state_string[shared_vars.c_token->type], shared_vars.c_token->data, shared_vars.c_token->data_size);
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
                return symtableInsertVariable(&shared_vars.loc_symtable, name, NULL);
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

        return shared_vars.ret_value;
    }
}
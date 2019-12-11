#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "errors.h"
#include "code_gen.h"
#include "token.h"

int gen_prog_start()
{
    fprintf(stdout, ".IFJcode19\n"
                    "DEFVAR GF@%%input_var\n"
                    "DEFVAR GF@%%temp_op1\n"
                    "DEFVAR GF@%%temp_op2\n"
                    "DEFVAR GF@%%temp_op3\n"
                    "DEFVAR GF@%%temp_op4\n"
                    "DEFVAR GF@%%ret_val\n"
                    "JUMP $$main\n"
                    "\n#vestavene funkce\n\n");

    //generovani vestavenych funkci
    gen_inputf_decl("inputs");
    gen_inputf_decl("inputi");
    gen_inputf_decl("inputf");
    gen_print_decl();
    gen_len_decl();
    gen_substr_decl();
    gen_ord_decl();
    gen_chr_decl();

    //main programu
    fprintf(stdout, "LABEL $$main\n\n");
    return 0;
}

void gen_def_var (char *name_var, bool local) {
    if (local)
        fprintf(stdout, "DEFVAR LF@%s\n"
                        "MOVE LF@%s nil@nil\n", name_var, name_var);
    else
        fprintf(stdout, "DEFVAR GF@%s\n"
                        "MOVE GF@%s nil@nil\n", name_var, name_var);
}


void gen_print_decl () {
    fprintf(stdout, "#definice funkce print()\n"
                    "LABEL print\n"
                    "PUSHFRAME\n"
                    "DEFVAR LF@%%argc\n"
                    "DEFVAR LF@str\n"
                    "DEFVAR LF@$ret_val\n"
                    "DEFVAR LF@%%tmp\n"
                    "POPS LF@%%argc\n"
                    "LABEL $print_loop\n"
                    "LT LF@%%tmp LF@%%argc int@1\n"
                    "JUMPIFEQ $print_loop_end LF@%%tmp bool@true\n"
                    "POPS LF@str\n"
                    "WRITE LF@str\n"
                    "WRITE string@\\032\n"
                    "SUB LF@%%argc LF@%%argc int@1\n"
                    "JUMP $print_loop\n"
                    "LABEL $print_loop_end\n"
                    "MOVE LF@$ret_val nil@nil\n"
                    "WRITE string@\\010\n"
                    "POPFRAME\n"
                    "RETURN\n\n");
}


void gen_inputf_decl (char *input_type) {
    fprintf(stdout, "#definice funkce %s()\n"
                    "LABEL %s\n", input_type, input_type);
    fprintf(stdout, "PUSHFRAME\n"
                    "DEFVAR LF@$ret_val\n"
                    "DEFVAR LF@%%argc\n"
                    "POPS LF@%%argc\n");
    if (strcmp(input_type, "inputs") == 0) {
        fprintf(stdout, "READ LF@$ret_val string\n");
    }
    else if (strcmp(input_type, "inputi") == 0) {
        fprintf(stdout, "READ LF@$ret_val int\n");
    }
    else {
        fprintf(stdout, "READ LF@$ret_val float\n");
    }
    fprintf(stdout, "POPFRAME\n"
                    "RETURN\n\n");
}

void gen_len_decl () {
    fprintf(stdout, "#definice funkce len()\n"
                    "LABEL len\n"
                    "PUSHFRAME\n"
                    "DEFVAR LF@arg\n"
                    "DEFVAR LF@$ret_val\n"
                    "DEFVAR LF@%%argc\n"
                    "POPS LF@%%argc\n"
                    "POPS LF@arg\n"
                    "STRLEN LF@$ret_val LF@arg\n"
                    "POPFRAME\n"
                    "RETURN\n\n");
}

void gen_chr_decl () {
    fprintf(stdout, "#definice funkce chr()\n"
                    "LABEL chr\n"
                    "PUSHFRAME\n"
                    "DEFVAR LF@i\n"
                    "DEFVAR LF@$ret_val\n"
                    "DEFVAR LF@%%argc\n"
                    "POPS LF@%%argc\n"
                    "POPS LF@i\n"
                    "INT2CHAR LF@$ret_val LF@i\n"
                    "POPFRAME\n"
                    "RETURN\n\n");
}


void gen_ord_decl () {
    fprintf(stdout, "#definice funkce ord()\n"
                    "LABEL ord\n"
                    "PUSHFRAME\n"
                    "DEFVAR LF@%%argc\n"
                    "POPS LF@%%argc\n"
                    "DEFVAR LF@s\n"
                    "POPS LF@s\n"
                    "DEFVAR LF@i\n"
                    "POPS LF@i\n"
                    "DEFVAR LF@slen\n"
                    "DEFVAR LF@$ret_val\n"
                    "PUSHS LF@s\n"
                    "PUSHS int@1\n"
                    "CREATEFRAME\n"
                    "CALL len\n"
                    "MOVE LF@slen TF@$ret_val\n"
                    "DEFVAR LF@if\n"
                    "# i < 0\n"
                    "LT LF@if LF@i int@0\n"
                    "JUMPIFEQ NULL_ORD LF@if bool@true\n"
                    "# i > slen\n"
                    "GT LF@if LF@i LF@slen\n"
                    "JUMPIFEQ NULL_ORD LF@if bool@true\n"
                    "GETCHAR LF@s LF@s LS@i\n"
                    "STRI2INT LF@$ret_val LF@s LF@i\n"
                    "JUMP END_ORD\n"
                    "LABEL NIL_ORD\n"
                    "MOVE LF@$ret_val nil@nil\n"
                    "LABEL END_ORD\n"
                    "POPFRAME\n"
                    "RETURN\n\n");
}

void gen_substr_decl () {
    fprintf(stdout, "#definice funkce substr()\n"
                    "LABEL substr\n"
                    "PUSHFRAME\n"
                    "DEFVAR LF@$ret_val\n"
                    "DEFVAR LF@s\n"
                    "DEFVAR LF@i\n"
                    "DEFVAR LF@n\n"
                    "DEFVAR LF@slen\n"
                    "DEFVAR LF@if\n"
                    "DEFVAR LF@char\n"
                    "DEFVAR LF@%%argc\n"
                    "POPS LF@%%argc\n"
                    "POPS LF@s\n"
                    "POPS LF@i\n"
                    "POPS LF@n\n"
                    "# CALLING LEN FUNCTION\n"
                    "PUSHS LF@s\n"
                    "PUSHS int@1\n"
                    "CREATEFRAME\n"
                    "CALL len\n"
                    "MOVE LF@slen TF@$ret_val\n"
                    "# i < 0\n"
                    "LT LF@if LF@i int@0\n"
                    "JUMPIFEQ NULL_SUBSTR LF@if bool@true\n"
                    "# i > slen\n"
                    "GT LF@if LF@i LF@slen\n"
                    "JUMPIFEQ NULL_SUBSTR LF@if bool@true\n"
                    "# n < 0\n"
                    "LT LF@if LF@n int@0\n"
                    "JUMPIFEQ NULL_SUBSTR LF@if bool@true\n"
                    "MOVE LF@$ret_val string@\n"
                    "JUMPIFEQ END_SUBSTR LF@n int@0\n"
                    "ADD LF@n LF@i LF@n\n"
                    "SUB LF@n LF@n int@1\n"
                    "# ALGORITHM\n"
                    "LABEL LOOP_SUBSTR\n"
                    "GETCHAR LF@char LF@s LF@i\n"
                    "CONCAT LF@$ret_val LF@$ret_val LF@char\n"
                    "ADD LF@i LF@i int@1\n"
                    "JUMPIFEQ END_SUBSTR LF@i LF@n\n"
                    "JUMPIFEQ END_SUBSTR LF@i LF@slen\n"
                    "JUMP LOOP_SUBSTR\n"
                    "JUMP END_SUBSTR\n"
                    "LABEL NULL_SUBSTR\n"
                    "MOVE LF@$ret_val nil@nil\n"
                    "LABEL END_SUBSTR\n"
                    "POPFRAME\n"
                    "RETURN\n\n");
}

char *create_var(t_state type, char *cont, bool local) {
    char *tmp;
    char *scope;

    switch(type) {
        case ID:
            if (strcmp(cont, "$ret_val") == 0)
                scope = "TF@";
            else if (local)
                scope = "LF@";
            else
                scope = "GF@";

            if ((tmp = malloc(3 + strlen(cont) + 1)) == NULL)
                return NULL;

            strcpy(tmp, scope);
            strcpy(tmp + 3, cont);
            return tmp;

        case INTEGER:
            if ((tmp = malloc(4 + strlen(cont) + 1)) == NULL)
                return NULL;
            strcpy(tmp, "int@");
            strcpy(tmp + 4, cont);
            return tmp;

        case DOUBLE:
            if ((tmp = malloc(6 + strlen(cont) + 1)) == NULL)
                return NULL;
            strcpy(tmp, "float@");
            strcpy(tmp + 6, cont);
            return tmp;

        case STR:
        case DOCSTR:
            if ((tmp = malloc(7 + strlen(cont) + 1)) == NULL)
                return NULL;
            strcpy(tmp, "string@");
            strcpy(tmp + 7, cont);
            return tmp;

        case KW_NONE:
            if ((tmp = malloc(8)) == NULL)
                return NULL;

            strcpy(tmp, "nil@nil");

            return tmp;

        default:
            return NULL;
    }
}

int gen_assign(char *dest_name, t_token src_token, bool local) {
    gen_type_control(ID, dest_name, src_token.type, src_token.data, local);
    char *dest = create_var(ID, dest_name, local);
    char *src = create_var(src_token.type, src_token.data, local);

    if (dest == NULL)
        return INTERNAL_ERROR;

    fprintf(stdout, "MOVE %s %s\n", dest, src);

    free(dest);
    return OK;
}

int gen_func_start(char *func_name) {
    static int counter = 0;

    fprintf(stdout, "JUMP $%s_skip%d\n", func_name, counter);
    fprintf(stdout, "LABEL %s\n", func_name);

    counter++;
    return OK;
}

int gen_if_start() {

}

int gen_else() {

}

int gen_if_end() {

}

int gen_func_arg(char *name) {
    fprintf(stdout, "POPS LF@%s\n", name);

    return OK;
}

int gen_func_end(char *func_name) {
    static int counter = 0;

    fprintf(stdout, "LABEL $%s_skip%d\n", func_name, counter);

    counter++;
    return OK;
}

int gen_func_call(char *name, int argc, stack *args_stack, bool local) {
    char *var;
    t_token *token;

    for (int i = 0; i < argc; i++) {
        token = stackTopPop(args_stack);
        var = create_var(token->type, token->data, local);

        fprintf(stdout, "PUSHS %s\n", var);

        free(token);
        free(var);
    }

    fprintf(stdout, "PUSHS int@%d\n", argc);
    fprintf(stdout, "CREATEFRAME\n"
                    "CALL %s\n", name);

    return OK;
}

int gen_type_control(t_state type1, char *cont1, t_state type2, char *cont2, bool local){
    static int counter= 0;
    char *var1 = create_var(type1, cont1, local);
    char *var2 = create_var(type2, cont2, local);

    if (var1 == NULL || var2 == NULL)
        return INTERNAL_ERROR;

    //generovani pretypovani pro ID
    char *var1_tmp, *var2_tmp;

    fprintf(stdout,"MOVE GF@%%temp_op1 %s\n"
                    "MOVE GF@%%temp_op2 %s\n"
                    "TYPE GF@%%temp_op3 GF@%%temp_op1\n"
                    "TYPE GF@%%temp_op4 GF@%%temp_op2\n"

                    "EQ GF@%%temp_op1 GF@%%temp_op3 string@int\n"
                    "EQ GF@%%temp_op2 GF@%%temp_op4 string@float\n"
                    "AND GF@%%temp_op1 GF@%%temp_op1 GF@%%temp_op2\n"
                    "JUMPIFNEQ $type_test_convert2_%d GF@%%temp_op1 bool@true\n", var1, var2, counter);
    if (type1 == ID)
        fprintf(stdout, "MOVE GF@%%temp_op1 %s\n"
                        "INT2FLOAT %s GF@%%temp_op1\n"
                        "TYPE GF@%%temp_op3 GF@%%temp_op1\n", var1, var1);

    fprintf(stdout,"LABEL $type_test_convert2_%d\n"
                   "EQ GF@%%temp_op1 GF@%%temp_op3 string@float\n"
                    "EQ GF@%%temp_op2 GF@%%temp_op4 string@int\n"
                    "AND GF@%%temp_op1 GF@%%temp_op1 GF@%%temp_op2\n"
                    "JUMPIFNEQ $type_test_check%d GF@%%temp_op1 bool@true\n", counter, counter);
    if (type2 == ID)
        fprintf(stdout, "MOVE GF@%%temp_op2 %s\n"
                        "INT2FLOAT %s GF@%%temp_op2\n"
                        "TYPE GF@%%temp_op4 GF@%%temp_op1\n", var2, var2);
    fprintf(stdout, "LABEL $type_test_check%d\n"
                    "JUMPIFEQ $type_test_ok%d GF@%%temp_op3 string@nil\n"
                    "JUMPIFEQ $type_test_ok%d GF@%%temp_op4 string@nil\n"
                    "JUMPIFEQ $type_test_ok%d GF@%%temp_op3 GF@%%temp_op4\n"
                    "EXIT int@4\n"
                    "LABEL $type_test_ok%d\n",counter, counter, counter, counter, counter);

    free(var1);
    free(var2);
    counter++;

    return OK;
}


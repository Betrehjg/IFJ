//
// Created by Samuel Spišák on 25/11/2019.
//

#include "psa.h"
#include "scanner.h"
#include "parser.h"
#include "string.h"
#include "code_gen.h"

/// Precedencna tabulka: R (>), L (<), E (=), N (error)

const int PSA_table[8][8] =
        {
         // 	|+- | */| \ | r | ( | ) | i | $ |
                { R , L , L , R , L , R , L , R }, // +-
                { R , R , R , R , L , R , L , R }, // */
                { R , L , R , R , L , R , L , R }, // //
                { L , L , L , N , L , R , L , R }, // = <> <= < >= >
                { L , L , L , L , L , E , L , N }, // (
                { R , R , R , R , N , R , N , R }, // )
                { R , R , R , R , N , R , N , R }, // i (int, double, string, id)
                { L , L , L , L , L , N , L , N }  // $
        };

/**
 * Prevod z symbol enumu na index tabulky.
 */

static PSA_table_index_enum get_index_from_symbol (PSA_symbol_enum symbol)
{
        switch (symbol)
        {
                case PLUS:
                case MINUS:
                        return T_PLUS_MINUS;

                case MUL:
                case DIV:
                        return T_MUL_DIV;

                case IDIV:
                        return T_IDIV;

                case EQ:
                case NEQ:
                case LEQ:
                case LESS:
                case MEQ:
                case MORE:
                        return T_RELATION;

                case BRACKET_LEFT:
                        return T_LEFT_BRACKET;

                case BRACKET_RIGHT:
                        return T_RIGHT_BRACKET;

                case IDENTIFIER:
                case INT_N:
                case DOUBLE_N:
                case STRING:
                case NONE:
                        return T_VALUE;

                default:
                        return T_DOLLAR;
        }
}

/**
 * Prevod z typu t_state na typ symbol ktory sa pouziva pri identifikaci pravidla
 */

static PSA_symbol_enum get_symbol_from_token(t_token* token)
{
        switch (token->type)
        {
                case ADD:
                        return PLUS;
                case SUBSTRACT:
                        return MINUS;
                case MULTIPLY:
                        return MUL;
                case DIVIDE:
                        return DIV;
                case IDIVIDE:
                        return IDIV;
                case LOG_EQ:
                        return EQ;
                case LOG_NONEQ:
                        return NEQ;
                case LOG_LEQ:
                        return LEQ;
                case LOG_LESS:
                        return LESS;
                case LOG_MEQ:
                        return MEQ;
                case LOG_MORE:
                        return MORE;
                case L_BRAC:
                        return BRACKET_LEFT;
                case R_BRAC:
                        return BRACKET_RIGHT;
                case ID:
                        return IDENTIFIER;
                case INTEGER:
                        return INT_N;
                case DOUBLE:
                        return DOUBLE_N;
                case STR:
                        return STRING;
                case KW_NONE:
                        return NONE;
                default:
                        return DOLLAR;
        }
}

/**
 * Najde a vrati prvy eItem zo stacku ktory je terminal
 * @param s
 * @return prvy terminal zo stacku
 */

static eItem * first_terminal (eStack *s){

        eItem *pom = eStackTopItem(s);
        while (!pom->is_terminal) {
                pom = pom->next;
        }
        return pom;
}

/**
 * zisti index prveho terminalu v stacku
 * @return firstTerminal index
 */

static PSA_table_index_enum terminal_index(eStack *s){

        return first_terminal(s)->index;
}

/**
 * Najde pravidlo podla ktoreho je mozne vyraz redukovat
 * @param symbolCount - pocet symbolov na redukciu, @param stack
 * @return najdene pravidlo
 */

static PSA_rules_enum check_rule (int symbolCount, eStack *pushdownS) {
        eItem * pom = eStackTopItem(pushdownS);
        switch (symbolCount)
        {
                case 1:
                        // E -> i

                        if (pom->table_symbol == IDENTIFIER || pom->table_symbol == INT_N
                        || pom->table_symbol == DOUBLE_N || pom->table_symbol == STRING || pom->table_symbol == NONE)
                               return OPERAND;

                        return INVALID_RULE;

                case 3:
                        // E -> (E)

                        if (pom->table_symbol == BRACKET_RIGHT
                        && !pom->next->is_terminal && pom->next->next->table_symbol == BRACKET_LEFT)
                                return LBR_E_RBR;

                        if (!pom->is_terminal && !pom->next->next->is_terminal)
                        {
                                switch (pom->next->table_symbol)
                                {
                                        // E -> E + E
                                        case PLUS:
                                                return E_PLUS_E;

                                        // E -> E - E
                                        case MINUS:
                                                return E_MINUS_E;

                                        // E -> E * E
                                        case MUL:
                                                return E_MUL_E;

                                        // E -> E / E
                                        case DIV:
                                                return E_DIV_E;

                                        // E -> E // E
                                        case IDIV:
                                                return E_IDIV_E;

                                        // E -> E = E
                                        case EQ:
                                                return E_EQ_E;

                                        // E -> E <> E
                                        case NEQ:
                                                return E_NEQ_E;

                                        // E -> E <= E
                                        case LEQ:
                                                return E_LEQ_E;

                                        // E -> E < E
                                        case LESS:
                                                return E_LESS_E;

                                        // E -> E >= E
                                        case MEQ:
                                                return E_MEQ_E;

                                        // E -> E > E
                                        case MORE:
                                                return E_MORE_E;

                                        // invalid rule
                                        default:
                                                return INVALID_RULE;
                                }
                        }
                        return INVALID_RULE;

                default:
                        return INVALID_RULE;
        }
}


/**
 * Funkcia na redukciu vyrazu podla najdeneho pravidla
 */

static int apply_rule(PSA_rules_enum rule, eStack *pushDownS, bool local) {
    static int counter = 0;
    eItem *tmp;
    char *tmp_name;
    switch (rule)
    {
        case OPERAND:

                ///Menim hodnotu/ id na neterminal


            if( eStackTopItem(pushDownS)->is_terminal){
                eStackTopItem(pushDownS)->action = E;
                eStackTopItem(pushDownS)->is_terminal = 0;

                //generovani prirazeni
                //TODO: kontrola mallocu .....
                eStackTopItem(pushDownS)->var_name = malloc(strlen("%%psa_tmp_var") + 1 + 30);
                sprintf(eStackTopItem(pushDownS)->var_name, "%%psa_tmp_var%d", counter++);
                gen_def_var(eStackTopItem(pushDownS)->var_name, local);
                gen_assign(eStackTopItem(pushDownS)->var_name, eStackTopItem(pushDownS)->src_token, local);
            } else return SYN_ERROR;
            return OK;

            // E -> (E)
        case LBR_E_RBR:
                
                pushDownS->top->next->next->table_symbol = pushDownS->top->next->table_symbol;

                eStackPop(pushDownS);
                eStackPop(pushDownS);
                eStackTopItem(pushDownS)->action = E;
                eStackTopItem(pushDownS)->is_terminal = 0;
                return OK;

        case E_PLUS_E:

            // E -> E - E
        case E_MINUS_E:

            // E -> E * E
        case E_MUL_E:

            // E -> E / E
        case E_DIV_E:

            // E -> E // E
        case E_IDIV_E:

            // E -> E = E
        case E_EQ_E:

            // E -> E <> E
        case E_NEQ_E:

            // E -> E <= E
        case E_LEQ_E:

            // E -> E < E
        case E_LESS_E:

            // E -> E >= E
        case E_MEQ_E:

            // E -> E > E
        case E_MORE_E:

            tmp = eStackTopItem(pushDownS);
            tmp_name = malloc(strlen("%%psa_tmp_var") + 1 + 30);
            sprintf(tmp_name, "%%psa_tmp_var%d", counter++);
            gen_def_var(tmp_name, local);
            gen_type_control(ID, tmp->next->next->var_name, ID, tmp->var_name, local);
            gen_aritm_op(rule, tmp->next->next->var_name, tmp->var_name, tmp_name, local);

            ///Pop 2 poloziek, poslednu nastavim na neterminal
            eStackPop(pushDownS);
            eStackPop(pushDownS);
            eStackTopItem(pushDownS)->action = E;
            eStackTopItem(pushDownS)->is_terminal = 0;
            eStackTopItem(pushDownS)->var_name = tmp_name;

            return OK;
            // invalid operator
        default:
                return SYN_ERROR;
    }
}
/**
 * Funkcia volana funkciou expression, vykonava reduckiu vyrazu podla pravidiel
 * @param inputStack - predavany vyraz
 * @param pushdownS  - stack v ktorom redukujem vyraz predavany z inputStacku
 * @return vysledok analyzy
 */

static int psAnalysis (eStack *inputS, eStack *pushdownS, bool local) {
        if (!isEmpty(inputS)) {

                PSA_operations_enum operation;
                PSA_symbol_enum input_Symbol;
                int ruleCount = 0;

                while (1) {

                        ///kontrola prazdneho input stacku - ak je prazdny je input "$"

                        if(!isEmpty(inputS)) {
                                 input_Symbol = eStackTopItem(inputS)->table_symbol;
                        } else input_Symbol = DOLLAR;


                        ///Ak je input prazdny a zaroven prvy terminal na pushdowne je "$", redukcia bola uspesna (OK)

                        if(input_Symbol == DOLLAR && first_terminal(pushdownS)->table_symbol == DOLLAR) return OK;

                        ///-------------------------------------------------------------------------------------------

                        if (input_Symbol == DOLLAR) {
                                operation = R;
                        } else if (first_terminal(pushdownS)->table_symbol == DOLLAR) {
                                operation = L;
                        } else {
                                operation = PSA_table[terminal_index(pushdownS)][terminal_index(inputS)];
                        }

                        switch (operation) {
                                case E:
                                        eStackTopItem(pushdownS)->action = E;
                                        eItemStackPush(pushdownS, eStackTopItem(inputS));
                                        eStackPop(inputS);

                                        break;

                                case L:
                                        if (input_Symbol == IDENTIFIER || input_Symbol == INT_N
                                            || input_Symbol == DOUBLE_N || input_Symbol == STRING
                                            || input_Symbol == BRACKET_LEFT || input_Symbol == NONE)
                                        {

                                                eItemStackPush(pushdownS, eStackTopItem(inputS));
                                                eStackPop(inputS);

                                                eStackTopItem(pushdownS)->action = L;
                                        }
                                        else
                                        {
                                                eStackTopItem(pushdownS)->action = L;

                                                eItemStackPush(pushdownS, eStackTopItem(inputS));
                                                eStackPop(inputS);
                                        }

                                        break;

                                case R:

                                        if (eStackTopItem(pushdownS)->action == L
                                            && eStackTopItem(pushdownS)->is_terminal) {
                                                ruleCount = 1;
                                        } else ruleCount = 3;
                                        
                                        PSA_rules_enum rule;
                                        rule = check_rule(ruleCount, pushdownS);

                                        int rule_result = apply_rule(rule, pushdownS, local);
                                        if(rule_result != OK) return rule_result;

                                        break;

                                case N:
                                        if(first_terminal(pushdownS)->table_symbol == DOLLAR && isEmpty(inputS)) return OK;

                                        return SYN_ERROR;
                        }
                }
        } else return SYN_ERROR; //prazdny vyraz
}

/**
 * Funkcia volana z parseru pri najdenu vyrazu
 * @param c_token
 * @return result
 */

int expression(t_token * current_token, bool local, t_token *src_token){

        int result;
        eStack * inputStack;
        eStack * pushDownStack;
        eStack * inputReverse;

        if((inputStack = (eStack *)malloc(sizeof(eStack))) == NULL) return INTERNAL_ERROR;
        if((pushDownStack = (eStack *)malloc(sizeof(eStack))) == NULL) return INTERNAL_ERROR;
        if((inputReverse = (eStack *)malloc(sizeof(eStack))) == NULL) return INTERNAL_ERROR;

        eStackInit(inputStack);
        eStackInit(pushDownStack);
        eStackInit(inputReverse);

        int exprSizeCount = 0;
        PSA_table_index_enum currIndex;


        //Nacitam do pola indexov cely vyraz, ukladam tokeny do stacku
        //na konci cyklu je nacitany prvy token nepatriaci do vyrazu

        while((currIndex = get_index_from_symbol(get_symbol_from_token(current_token)))!= T_DOLLAR){

                exprSizeCount++;
                eStackPush(inputStack, current_token, currIndex, get_symbol_from_token(current_token), 1);

                if (current_token->type == ID) {
                    COPY_TOKEN_DATA(name);
                    int ret = sem_var_test(name, local, NULL, false);
                    if (ret != OK)
                        return ret;
                }

                GET_TOKEN();
        }

        //nahram koncovy token (DOLLAR) nepatriaci vyrazu

        eStackPush(pushDownStack, current_token, currIndex, get_symbol_from_token(current_token), 1); //na pushdowne je DOLLAR

        //z input stacku v opacnom poradi naskladam tokeny nech ma vyraz spravne poradie
        do {
                eItemStackPush(inputReverse, eStackTopItem(inputStack));
                eStackPop(inputStack);

        } while(!isEmpty(inputStack));


        //samotna analyza s redukovanim vyrazu sa ulozi do result

        result = psAnalysis(inputReverse, pushDownStack, local);

        if (result == OK)
        {
            src_token->data = eStackTopItem(pushDownStack)->var_name;
            src_token->type = ID;
            src_token->data_size = strlen(src_token->data);
        }

        eStackDelete(inputStack);
        eStackDelete(inputReverse);
        eStackDelete(pushDownStack);

        return result;
}

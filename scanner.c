//
// Created by Matej Dubec on 21.11.2019.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "scanner.h"
#include "token.h"

/// FUNKCIA NA ZISTENIE KLUCOVEHO SLOVA
t_state token_keyword (char *str) {
    if (strcmp(str, "def") == 0) {
        return KW_DEF;
    }
    else if (strcmp(str, "while") == 0) {
        return KW_WHILE;
    }
    else if (strcmp(str, "if") == 0) {
        return KW_IF;
    }
    else if (strcmp(str, "else") == 0) {
        return KW_ELSE;
    }
    else if (strcmp(str, "None") == 0) {
        return KW_NONE;
    }
    else if (strcmp(str, "pass") == 0) {
        return KW_PASS;
    }
    else if (strcmp(str, "return") == 0) {
        return KW_RETURN;
    }
    else return ID;
}

int get_token(FILE *buffer, t_token *current_token, ind_stack *indentStack, int *newline) {

    static t_state previous_state = EOL;
    t_state current_state = START;
    static int end_of_file = 0;
    static int s_dedent = -1;
    static int empty_line = 1;

    int character;
    char hex[3];
    int whitespace = 0;
    int exponent = 0;
    int zeroIntVal = 0;
    int dededentPopCount = 0;

    init_token(current_token);
    while (1) {
        character = fgetc(buffer);

        if (end_of_file == 1) {
            current_state = LEX_EOF;
        }

        //opakovany dedent
        if (s_dedent >= 0) { //uroven na kterou se chci dostat je vetsi nez 0, jinak nema smysl resit dedent
            current_state = INDENT;
            whitespace = s_dedent;
        }

        /// KONECNY AUTOMAT
        switch (current_state) {
            case START:
                if(*newline == 1) {
                    if (character == ' ')
                        whitespace++;
                    else
                        ungetc(character, buffer);

                    current_state = INDENT;

                }else if (character == EOF) {
                    //dedent na konci souboru
                    ungetc(character, buffer);
                    current_state = INDENT;
                    end_of_file = 1;
                    s_dedent = 0;
                }
                else if (character == '\n') {
                    if ((add_char_token(current_token, '\n')) != OK) return INTERNAL_ERROR;
                    current_state = EOL;
                }
                else if (isspace(character)) {
                    whitespace++;
                    current_state = START;
                }
                else if (character == '#') {
                    current_state = LINE_COMM;
                }
                else if (character == '=') {
                    if (previous_state == EOL && whitespace == 0) return LEX_ERROR; // JE TOTO LEX ERROR? osetri co ak previous t_state nieje premenna / ID ?
                    else {
                        if ((add_char_token(current_token, '=')) != OK) return INTERNAL_ERROR;
                        current_state = ASSIGN;
                    }
                }
                else if (character == '!') {
                    if ((add_char_token(current_token, '!')) != OK) return INTERNAL_ERROR;
                    current_state = EXCLAIM;
                }
                else if (character == ',') {
                    if ((add_char_token(current_token, ',')) != OK) return INTERNAL_ERROR;
                    current_state = COMMA;
                }
                else if (character == '+') {
                    if ((add_char_token(current_token, '+')) != OK) return INTERNAL_ERROR;
                    current_state = ADD;
                }
                else if (character == '-') {
                    if ((add_char_token(current_token, '-')) != OK) return INTERNAL_ERROR;
                    current_state = SUBSTRACT;
                }
                else if (character == '*') {
                    if ((add_char_token(current_token, '*')) != OK) return INTERNAL_ERROR;
                    current_state = MULTIPLY;
                }
                else if (character == '/') {
                    if ((add_char_token(current_token, '/')) != OK) return INTERNAL_ERROR;
                    current_state = DIVIDE;
                }
                else if (character == '(') {
                    if ((add_char_token(current_token, '(')) != OK) return INTERNAL_ERROR;
                    current_state = L_BRAC;
                }
                else if (character == ')') {
                    if ((add_char_token(current_token, ')')) != OK) return INTERNAL_ERROR;
                    current_state = R_BRAC;
                }
                else if (character == '<') {
                    if ((add_char_token(current_token, '<')) != OK) return INTERNAL_ERROR;
                    current_state = LOG_LESS;
                }
                else if (character == '>') {
                    if ((add_char_token(current_token, '>')) != OK) return INTERNAL_ERROR;
                    current_state = LOG_MORE;
                }
                else if (isalpha(character) || character == '_') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = ID;
                }
                else if (isdigit(character)) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    if (character == 0) {
                        zeroIntVal = 1; // uz som zadal 0, dalsia v ramci jedneho cisla hned za sebou byt nemoze
                    }
                    current_state = INTEGER;
                }
                else if (character == '\'') {
                    current_state = STR_INPUT;
                }
                else if (character == '\"') {
                    current_state = DOCSTR_BEGIN;
                }
                else if (character == ':') {
                    if ((add_char_token(current_token, ':')) != OK) return INTERNAL_ERROR;
                    current_state = COLON;
                }
                else return LEX_ERROR;
                previous_state = START;
                break;

            case INDENT:
                *newline = 0;
                s_dedent = -1;
                if (character == ' ') {
                    whitespace += 1;
                    current_state = INDENT;
                }
                else{
                    if (character == '#') {
                        current_state = LINE_COMM;
                        break;
                    }

                    if (empty_line && character == '\n') {
                        current_state = START;
                        *newline = 1;
                        break;
                    }

                    if (whitespace == indentStack->top->indent) { // je toto ok porovnanie?
                        ungetc(character, buffer); //musim vracet nacty znak
                        current_state = START;
                    }
                    else if (whitespace > indentStack->top->indent) {
                        indentStackPush(indentStack, whitespace);
                        add_indent(current_token, whitespace);
                        return_token(current_state, current_token, character, buffer, &empty_line);
                        return OK;
                    }
                    else if (whitespace < indentStack->top->indent) {

                        /// GENEROVANIE DEDENTU
                        current_state = DEDENT;

                        if ((indentStack->top->next == NULL) && (whitespace != indentStack->top->indent)) return LEX_ERROR;
                        if (whitespace > indentStack->top->next->indent) return LEX_ERROR; //mam dedent ale na spatnou uroven

                        /// GENEROVANIE DEDENTU, OK INDENTACIA
                        add_indent(current_token, dededentPopCount); // V PRIPADE DEDENTU UKLADAM DO DAT POCET NUTNYCH POPOV
                        return_token(current_state, current_token, character, buffer, &empty_line);
                        indentStackPop(indentStack);
                        s_dedent = whitespace;
                        return OK;
                    }
                }
                break;

            case LINE_COMM:
                if (character == '\n') {
                    current_state = START;

                    if (!empty_line)
                        ungetc(character, buffer);
                    else
                        *newline = 1;

                }
                else current_state = LINE_COMM;
                break;

            case DOCSTR_BEGIN:
                if (character == '"') {
                    if ((character = fgetc(buffer)) == '"' ) {
                        current_state = DOCSTR_INPUT;
                    }
                    else return LEX_ERROR;
                }
                else return LEX_ERROR;
                break;

            case DOCSTR_INPUT:
                if (character == '\n') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOCSTR_INPUT;
                }
                else if (character == '\\'){
                    current_state = DOCSTR_BCKSLSH;
                }
                else if (character == EOF) return LEX_ERROR;
                else if (character == '"') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOCSTR_END;
                }
                else  {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOCSTR_INPUT;
                }
                break;

            case DOCSTR_BCKSLSH:
                current_state =  DOCSTR_INPUT;
                if (character == '"') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '4')) != OK) return INTERNAL_ERROR;
                }
                else if (character == 'n') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '1')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                }
                else if (character == 't') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                }
                else if (character == '\'') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                }
                else if (character == '\\') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '2')) != OK) return INTERNAL_ERROR;
                }
                else if (character > 31) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                }
                else return LEX_ERROR;
                break;

            case DOCSTR_END:
                if (character == '"') {
                    if ((character = fgetc(buffer)) == '"' ) {
                        current_state = DOCSTR;
                    }
                    else return LEX_ERROR;
                }
                else return LEX_ERROR;
                break;

            case DOCSTR: // iba return token
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case STR_INPUT:
                if (character == '\\') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    current_state = STR_BCKSLSH;
                }
                else if (character == '\'') {
                    current_state = STR;
                }
                else if (character == '#') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '5')) != OK) return INTERNAL_ERROR;
                }
                else if (character == ' ') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '2')) != OK) return INTERNAL_ERROR;
                }
                else if (character > 31) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = STR_INPUT;
                }
                else return LEX_ERROR;
                break;

            case STR:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case STR_BCKSLSH:
                current_state = STR_INPUT;
                if (character == 'x') {
                    current_state = STR_HEX;
                }
                else if (character == '"') {
                    if ((add_char_token(current_token, '"')) != OK) return INTERNAL_ERROR;
                }
                else if (character == 'n') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '1')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                }
                else if (character == 't') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                }
                else if (character == '\'') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                }
                else if (character == '\\') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '2')) != OK) return INTERNAL_ERROR;
                }
                else if (character > 31) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                }
                else return LEX_ERROR;
                break;

            case STR_HEX:
                if(isxdigit(character)) {
                    hex[0] = (char) character;
                    current_state = STR_HEX_2;
                }
                else return LEX_ERROR;
                break;

            case STR_HEX_2:
                if(isxdigit(character)) {
                    hex[1] = (char) character;
                    hex[2] = '\0';
                    if ((add_char_token(current_token, (char)strtol(hex, NULL, 16))) != OK) return INTERNAL_ERROR;
                    current_state = STR_INPUT;
                }

                else return LEX_ERROR;
                break;

            case EXPR_T:
                if(character == '+' || character == '-') { // ak po e/E ide volitelne  +/-
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = EXPR_S;
                }
                else if (isdigit(character)) { // ak napr 1.045e3 -> ungetnem 3ku, a prejdem do stavu, pre exponent, ktory striktne ocakava cisla
                    ungetc(character, buffer);
                    current_state = EXPR_S;
                }
                else return LEX_ERROR;
                break;

            case EXPR_S:
                if(isdigit(character)) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOUBLE;
                    exponent = 1;
                }
                else return LEX_ERROR;
                break;

            case DOT:
                if (isdigit(character)) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOUBLE;
                } else return LEX_ERROR; // MAM NACITANE NAPR: 8."ak na tejto pozicii nieje cislo, je to error."
                break;

            case EXCLAIM:
                if (character == '=') {
                    current_state = LOG_NONEQ;
                }
                else return LEX_ERROR; //LEXICAL
                break;

            case ID:
                if((isalnum(character)) || character == '_') { // pokud je pismeno, cislo nebo podrzitko
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = ID;
                }
                else {
                    add_char_token(current_token, '\0');
                    current_state = token_keyword(current_token->data); // Zjisteni jestli se jedna o klicove slovo
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case INTEGER:
                if (isdigit(character)) {
                    if (zeroIntVal == 1) return LEX_ERROR; //  pracujem s hodnotou 0x; kde x je dalsie cislo -> zly zapis cisla
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                }
                else if (character == '.') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOT;
                }
                else if (character == 'e' || character == 'E') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = EXPR_T;
                }
                else {
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case DOUBLE:
                if(isdigit(character)) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOUBLE;
                }
                else if ((character == 'e' || character == 'E') && exponent == 0) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = EXPR_T;
                }
                else {
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case LOG_EQ:
            case LOG_NONEQ:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case LOG_MORE:
                if (character == '=') {
                    current_state = LOG_MEQ;
                }
                else {
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case LOG_LESS:
                if (character == '=') {
                    current_state = LOG_LEQ;
                }
                else {
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case LOG_LEQ:
            case LOG_MEQ:
            case LOG_NEQ:
            case ADD:
            case SUBSTRACT:
            case MULTIPLY:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case DIVIDE:
                if (character == '/') {
                    if ((add_char_token(current_token, '/')) != OK) return INTERNAL_ERROR;
                    current_state = IDIVIDE;
                }
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case IDIVIDE:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case EOL:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                *newline = 1;
                empty_line = 1;
                return OK;

            case L_BRAC:
            case R_BRAC:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case ASSIGN:
                if (character == '=') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = LOG_EQ;
                }
                else {
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case COMMA:
            case COLON:
            case LEX_EOF:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            default: // internal
                return INTERNAL_ERROR;
        }
    }
    return OK;
}
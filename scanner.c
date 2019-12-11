//
// Created by Matej Dubec on 21.11.2019.
// xdubec00 - IFJ SCANNER
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
/// HLAVNY LOOP
//  Funkcia, volana v parsri, pozostava z cyklickeho citania znakov zo standardneho vstupu,
//  dokym sa nam nepodari nacitat nejaky validny lexem
int get_token(FILE *buffer, t_token *current_token, ind_stack *indentStack, int *newline) {
    // pomocne premenne
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
        if (s_dedent >= 0) { // Uroven na ktoru sa chcem dostat je vacsia nez 0, inak nema zmysel riesit dedent
            current_state = INDENT;
            whitespace = s_dedent;
        }

        /// KONECNY AUTOMAT
        //  Podla prveho nacitaneho znaku urcim, do akeho stavu sa dostanem, akonahle som v nejakom konkretnom stave,
        //  kontrolujem, ci nacitane znaky vyhovuju udajnemu typu tokenu
        //  character je nacitany znak
        switch (current_state) {
            case START: //  ZACIATOCNY STAV TOKENU
                if(*newline == 1) {
                    if (character == ' ')
                        whitespace++;
                    else
                        ungetc(character, buffer);

                    current_state = INDENT;

                }else if (character == EOF) { // Dedent na konci suboru                
                    ungetc(character, buffer);
                    current_state = INDENT;
                    end_of_file = 1;
                    s_dedent = 0;
                }
                else if (character == '\n') {
                    if ((add_char_token(current_token, '\n')) != OK) return INTERNAL_ERROR; // pridam znak do struktury pre token
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
                    if (previous_state == EOL && whitespace == 0) return LEX_ERROR; 
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
                else if (isalpha(character) || character == '_') { // Identifikator -> funkcia / premenna
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = ID;
                }
                else if (isdigit(character)) { // Cislo
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    if (character == '0') {
                        zeroIntVal = 1; // Uz som zadal 0, dalsia 0 v ramci jedneho cisla hned za sebou byt nemoze
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

            case INDENT: // Riesenie indentacie podla zadania
                *newline = 0;
                s_dedent = -1;
                if (character == ' ') { // Zvysenie indentacie
                    whitespace += 1;
                    current_state = INDENT;
                }
                else{
                    if (character == '#') { // Komentar
                        current_state = LINE_COMM;
                        break;
                    }

                    if (empty_line && character == '\n') { // Znak konca riadku
                        current_state = START;
                        *newline = 1;
                        break;
                    }

                    if (whitespace == indentStack->top->indent) {
                        ungetc(character, buffer); // Musim vracat nacitany znak
                        current_state = START;
                    }
                    else if (whitespace > indentStack->top->indent) {
                        indentStackPush(indentStack, whitespace);
                        add_indent(current_token, whitespace);
                        return_token(current_state, current_token, character, buffer, &empty_line);  // Vraciam kompletne nacitany token
                        return OK;
                    }
                    else if (whitespace < indentStack->top->indent) {

                        /// GENEROVANIE DEDENTU
                        current_state = DEDENT;

                        if ((indentStack->top->next == NULL) && (whitespace != indentStack->top->indent)) return LEX_ERROR;
                        if (whitespace > indentStack->top->next->indent) return LEX_ERROR; // Mam dedent ale na zlu uroven

                        /// GENEROVANIE DEDENTU, OK INDENTACIA
                        add_indent(current_token, dededentPopCount); // V pripade dedentu ukladam do dat pocet nutnych popov
                        return_token(current_state, current_token, character, buffer, &empty_line);
                        indentStackPop(indentStack);
                        s_dedent = whitespace;
                        return OK;
                    }
                }
                break;

            case LINE_COMM: // Riadkovy komentar
                if (character == '\n') {
                    current_state = START;

                    if (!empty_line)
                        ungetc(character, buffer);
                    else
                        *newline = 1;

                }
                else current_state = LINE_COMM;
                break;

            case DOCSTR_BEGIN: // Dokumentacny retazec, skrz spravnu kontrolu vstupnych a vystupnych uvodzoviek, je rozdeleny na  viac stavov
                if (character == '"') {
                    if ((character = fgetc(buffer)) == '"' ) {
                        current_state = DOCSTR_INPUT; //  Boli zadane """ takze viem,  ze rozpoznavam token DOCSTR
                    }
                    else return LEX_ERROR;
                }
                else return LEX_ERROR;
                break;

            case DOCSTR_INPUT: // Samotny dokumentacny retazec
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

            case DOCSTR_BCKSLSH: // Duplicitny stav z klasickeho retazca
                current_state =  DOCSTR_INPUT;
                if (character == '"') {
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '4')) != OK) return INTERNAL_ERROR;
                }
                else if (character == 'n') { // \n
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '1')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                }
                else if (character == 't') { // \t
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                }
                else if (character == '\'') { // \t
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '3')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                }
                else if (character == '\\') {// \\ 
                    if ((add_char_token(current_token, '\\')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '0')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '9')) != OK) return INTERNAL_ERROR;
                    if ((add_char_token(current_token, '2')) != OK) return INTERNAL_ERROR;
                }
                else if (character > 31) {// Znak ktory mozem nacitat ako taky
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                }
                else return LEX_ERROR;
                break;

            case DOCSTR_END: // Koniec dokumentacneho retazca
                if (character == '"') {
                    if ((character = fgetc(buffer)) == '"' ) {
                        current_state = DOCSTR;
                    }
                    else return LEX_ERROR;
                }
                else return LEX_ERROR;
                break;

            case DOCSTR: // Stav iba vrati token, koncovy stav
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case STR_INPUT: // Zadavanie samotneho retazca
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

            case STR: // Retazec je pripraveny na vratenie kompletneho tokenu
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case STR_BCKSLSH: // V retazci bol zadany escape znak (/)
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

            case STR_HEX: // Hexadecimalne zadane cislo
                if(isxdigit(character)) {
                    hex[0] = (char) character; // Cele cislo zadane hexadecimalne ulozim do trojprvkoveho pola, pridam do obsahu tokenu
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

            case EXPR_T: // Desatinne cislo s exponentom
                if(character == '+' || character == '-') { // ak po e/E ide volitelne  +/-
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = EXPR_S;
                }
                else if (isdigit(character)) { // Ak napr 1.045e3 -> vratim na vstup 3, a prejdem do stavu pre exponent, ktory striktne ocakava cisla
                    ungetc(character, buffer);
                    current_state = EXPR_S;
                }
                else return LEX_ERROR;
                break;

            case EXPR_S: // Vedecky format (e / E)
                if(isdigit(character)) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOUBLE;
                    exponent = 1;
                }
                else return LEX_ERROR;
                break;

            case DOT:  // Nacital som . v exponente
                if (isdigit(character)) {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = DOUBLE;
                } else return LEX_ERROR; // Mam nacitane cislo vo forme napr. 8. , ak character nieje cislo -> chyba
                break;

            case EXCLAIM: // ! - negacia
                if (character == '=') {
                    current_state = LOG_NONEQ;
                }
                else return LEX_ERROR; //LEXICAL
                break;

            case ID: // Stav pre identifikator premennej / funkcie
                if((isalnum(character)) || character == '_') { // Ak je pismeno, cislo nebo podrzitko
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = ID;
                }
                else {
                    add_char_token(current_token, '\0');
                    current_state = token_keyword(current_token->data); // Zistenie ci sa jedna o klucove slovo
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case INTEGER: // Cele cislo
                if (isdigit(character)) {
                    if (zeroIntVal == 1) return LEX_ERROR; //  pracujem s hodnotou 0x; kde x je dalsie cislo -> zly zapis cisla
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                }
                else if (character == '.') {  // Exponent
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

            case DOUBLE: // Desatinne cislo
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
            /// LOGICKE POROVNANIA
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
            // TOKENY OPERATOROV
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
            
            case EOL: // Nacitany koniec riadku
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                *newline = 1;
                empty_line = 1;
                return OK;

            case L_BRAC:   // Prava a lava zatvorka
            case R_BRAC:
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            case ASSIGN: // Priradenie hodnoty
                if (character == '=') {
                    if ((add_char_token(current_token, character)) != OK) return INTERNAL_ERROR;
                    current_state = LOG_EQ; // Ak som nacital este jeden znak = -> Token porovnania (==)
                }
                else {
                    previous_state = current_state;
                    return_token(current_state, current_token, character, buffer, &empty_line);
                    return OK;
                }
                break;

            case COMMA:
            case COLON:
            case LEX_EOF: // Koniec suboru
                previous_state = current_state;
                return_token(current_state, current_token, character, buffer, &empty_line);
                return OK;

            default: // internal
                return INTERNAL_ERROR;
        }
    }
    return OK;
}

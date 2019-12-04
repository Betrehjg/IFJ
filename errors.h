//
// Created by Matej Dubec on 23.11.2019.
//

#ifndef PROJEKT_ERRORS_H
#define PROJEKT_ERRORS_H


#define OK 0
#define LEX_ERROR 1 /* chybna struktura lexemu, nespravny pocet medzier odsadenia */
#define SYN_ERROR 2 /* chybna syntax programu, neocakavana zmena urovne odsadenia */
#define SEM_ERROR 3 /* nedefinovana funkcia / premenna, pokus o redefiniciu funkcie / premennej */
#define TYPE_ERROR 4 /* semanticka / behova chyba typovej kompatibility v aritm. / retazc. / relacnych vyrazoch */
#define SEM_PCOUNT_ERROR 5 /* nespravny pocet parametrov pri volani funkcie */
#define SEM_OTHER_ERROR 6 /* ostatne semanticke chyby */
#define RUNTIME_ERROR 9 /* behova chyba pri  deleni nulou */
#define INTERNAL_ERROR 99 /* interna chyba, neovplyvnena vstup. programom (alokacia pam.) */
#define ERROR (-1)

#endif //PROJEKT_ERRORS_H
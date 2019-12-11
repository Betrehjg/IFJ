//
// Autor: Malínek Libor, Listopad 2019
//

#include "symtable.h"
#include "errors.h"
#include "string.h"
#include <stdlib.h>

void BSTInit(tBSTNodePtr *RootPtr)
{
    *RootPtr = NULL;
}

int BSTInsert(tBSTNodePtr *RootPtr, char *Key, void *Content, tNodeType typ)
{
    if(*RootPtr == NULL)
    {
        tBSTNodePtr pomocna;
        pomocna = malloc(sizeof(struct tBSTNode));
        if(pomocna == NULL)
            return INTERNAL_ERROR;

        pomocna->Key = malloc(strlen(Key) + 1);
        if (pomocna->Key == NULL)
            return INTERNAL_ERROR;

        pomocna->nodeType = typ;
        pomocna->BSTNodeCont = Content;
        strcpy(pomocna->Key, Key);
        pomocna->LPtr = NULL;
        pomocna->RPtr = NULL;
        *RootPtr = pomocna;
    }
    else
    {
        int res = strcmp((*RootPtr)->Key, Key);
        if(res == 0) //pokud vkladame prvek se stejnym klicem, aktualizujeme
        {
            (*RootPtr)->BSTNodeCont = Content;
        }
        else if (res < 0) // pokud ma vkladany prvek mensi hodnotu klice, jdeme do leva
        {
            BSTInsert(&(*RootPtr)->LPtr, Key, Content, typ);
        }
        else if (res > 0) //pokud ma vkladany prvek vetsi hodnotu klice, jdeme do prava
        {
            BSTInsert(&(*RootPtr)->RPtr, Key, Content, typ);
        }
    }

    return OK;
}

tBSTNodePtr BSTSearch(tBSTNodePtr RootPtr, char *Key)
{
    if(RootPtr == NULL)
    {
        return NULL;
    }
    else
    {
        int res = strcmp(RootPtr->Key, Key);
        if(res == 0) //pokud je klic na aktualnim prvku roven hledanemu => nasli jsme hledany prvek
        {
            return RootPtr;
        }
        else if (res > 0) //pokud je klic na aktualnim prvku mensi, nez hledany, jdeme do prava
        {
            return (BSTSearch(RootPtr->RPtr,Key));
        }
        else if (res < 0)//pokud je klic na aktaulnim prvni vetsi, nez hledany ,jdeme do leva
        {
            return(BSTSearch(RootPtr->LPtr,Key));
        }
    }
    return NULL;
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) //pomocna funkce
{
    if((*RootPtr) != NULL)
    {

        if((*RootPtr)->RPtr != NULL) //jdeme porad do prava
        {
            ReplaceByRightmost(PtrReplaced,&(*RootPtr)->RPtr);
        }
        else //jsme na prvku na nejpravejsim miste
        {
            PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;
            PtrReplaced->Key = (*RootPtr)->Key;
            tBSTNodePtr pomocna = *RootPtr;
            *RootPtr = (*RootPtr)->LPtr;
            free(pomocna->BSTNodeCont);
            free(pomocna);
        }
    }

}

void BSTDelete(tBSTNodePtr *RootPtr, char *Key) //odstranit podle key
{
    if((*RootPtr) == NULL)
    {
        return;
    }
    else
    {
        if((*RootPtr)->Key == Key) //jsme na prvku, ktery chceme uvolnit
        {
            tBSTNodePtr pomocna = (*RootPtr);
            if((*RootPtr)->LPtr == NULL) //ma pouze praveho potomka
            {
                *RootPtr = pomocna->RPtr;
                free(pomocna->BSTNodeCont);
                free(pomocna);
            }
            else if((*RootPtr)->RPtr == NULL)//ma pouze leveho potomka
            {
                *RootPtr = pomocna->LPtr;
                free(pomocna->BSTNodeCont);
                free(pomocna);
            }
            else //ma dva potomky
            {
                ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
            }

        }
        else if((*RootPtr)->Key < Key)
        {
            BSTDelete(&(*RootPtr)->RPtr, Key);
        }
        else if((*RootPtr)->Key > Key)
        {
            BSTDelete(&(*RootPtr)->LPtr, Key);
        }


    }
}

void BSTDispose(tBSTNodePtr *RootPtr) //vymazat cely strom
{
    if((*RootPtr) == NULL)
    {
        return;
    }
    else
    {
        BSTDispose(&(*RootPtr)->LPtr);
        BSTDispose(&(*RootPtr)->RPtr);
        free((*RootPtr)->BSTNodeCont);
        free(*RootPtr);
        *RootPtr = NULL;
        BSTInit(RootPtr);
    }
}

void symtableInit(tSymtable *TablePtr) //inicializace tabulky symbolu
{
    BSTInit(&(TablePtr->root));
}

int symtableInsertVariable(tSymtable *TablePtr, char *Key, void *content) //pridat promennou
{
    inVariable *x = malloc(sizeof(inVariable));
    if (x == NULL)
        return INTERNAL_ERROR;

    x->argContent = content;

    return BSTInsert(&(TablePtr->root), Key, x, ndVariable);
}

int symtableInsertFunction(tSymtable *TablePtr, char *Key, bool defined, int argc) //pridat funkci
{
    inFunction *x = malloc(sizeof(inFunction));
    if (x == NULL)
        return INTERNAL_ERROR;

    x->defined = defined;
    x->argContent = argc;

    return BSTInsert(&(TablePtr->root), Key, x, ndFunction);
}

void symtableDelete(tSymtable *TablePtr, char *Key) //odstranit konkretni
{
    BSTDelete(&(TablePtr->root), Key);
}

void symtableDispose(tSymtable *TablePtr) //vymazat cely 
{
    BSTDispose(&(TablePtr->root));
}

tBSTNodePtr symtableSearch(tSymtable *TablePtr, char *Key) //hledat
{
    return BSTSearch(TablePtr->root, Key);
}

/**
 * @file symtab.h
 * @brief Interface da Tabela de Simbolos com suporte a pilha de escopos
 */

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

#define SIZE 211

typedef struct LineListRec {
    int lineno;
    struct LineListRec* next;
} *LineList;

typedef struct BucketListRec {
    char* name;
    LineList lines;
    int memloc;
    ExpType type;
    struct BucketListRec* next;
} *BucketList;

typedef struct ScopeListRec {
    char* scopeName;
    BucketList hashTable[SIZE];
    struct ScopeListRec* parent;
    struct ScopeListRec* next;
    int nestedLevel;
} *ScopeList;

/**
 * @brief Empilha um novo escopo
 * @param scopeName Nome do escopo (funcao ou bloco)
 * @return Ponteiro para o novo escopo criado
 */
ScopeList st_push_scope(char* scopeName);

/**
 * @brief Desempilha o escopo atual
 */
void st_pop_scope(void);

/**
 * @brief Insere um simbolo no escopo atual
 * @param name Nome do identificador
 * @param type Tipo do identificador
 * @param lineno Linha de declaracao
 * @param memloc Localizacao na memoria
 */
void st_insert(char* name, ExpType type, int lineno, int memloc);

/**
 * @brief Busca um simbolo em todos os escopos (do atual ate o global)
 * @param name Nome do identificador
 * @return Ponteiro para o bucket ou NULL se nao encontrado
 */
BucketList st_lookup(char* name);

/**
 * @brief Busca um simbolo apenas no escopo atual
 * @param name Nome do identificador
 * @return Ponteiro para o bucket ou NULL se nao encontrado
 */
BucketList st_lookup_top(char* name);

/**
 * @brief Entra em um escopo existente sem criar novo
 * @param scopeName Nome do escopo a entrar
 */
void st_enter_scope(char* scopeName);

/**
 * @brief Imprime a tabela de simbolos completa
 */
void printSymTab(void);

#endif

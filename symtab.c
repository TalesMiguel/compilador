/**
 * @file symtab.c
 * @brief Implementacao da Tabela de Simbolos com pilha de escopos
 */

#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ScopeList scopeStack = NULL;
static ScopeList allScopes = NULL;

/**
 * @brief Funcao hash
 * @param key String que sera convertida em indice
 * @return Indice da tabela hash
 */
static int hash(char* key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << 4) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/**
 * @brief Empilha um novo escopo
 * @param scopeName Nome do escopo (funcao ou bloco)
 * @return Ponteiro para o novo escopo criado
 */
ScopeList st_push_scope(char* scopeName) {
    ScopeList newScope = (ScopeList)malloc(sizeof(struct ScopeListRec));
    int i;
    if (newScope == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para novo escopo\n");
        return NULL;
    }
    newScope->scopeName = scopeName;
    for (i = 0; i < SIZE; i++)
        newScope->hashTable[i] = NULL;
    newScope->parent = scopeStack;
    newScope->nestedLevel = (scopeStack == NULL) ? 0 : scopeStack->nestedLevel + 1;
    newScope->next = allScopes;
    allScopes = newScope;
    scopeStack = newScope;
    return newScope;
}

/**
 * @brief Desempilha o escopo atual
 */
void st_pop_scope(void) {
    if (scopeStack != NULL) {
        scopeStack = scopeStack->parent;
    }
}

/**
 * @brief Insere um simbolo no escopo atual
 * @param name Nome do identificador
 * @param type Tipo do identificador
 * @param lineno Linha de declaracao
 * @param memloc Localizacao na memoria
 */
void st_insert(char* name, ExpType type, int lineno, int memloc) {
    int h;
    BucketList l;
    if (scopeStack == NULL) {
        fprintf(stderr, "Erro: nenhum escopo ativo para inserir simbolo\n");
        return;
    }
    h = hash(name);
    l = scopeStack->hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) {
        l = (BucketList)malloc(sizeof(struct BucketListRec));
        if (l == NULL) {
            fprintf(stderr, "Erro de alocacao de memoria na insercao de simbolo\n");
            return;
        }
        l->name = name;
        l->type = type;
        l->lines = (LineList)malloc(sizeof(struct LineListRec));
        if (l->lines == NULL) {
            fprintf(stderr, "Erro de alocacao de memoria para linha\n");
            return;
        }
        l->lines->lineno = lineno;
        l->memloc = memloc;
        l->lines->next = NULL;
        l->next = scopeStack->hashTable[h];
        scopeStack->hashTable[h] = l;
    } else {
        LineList t = l->lines;
        while (t->next != NULL)
            t = t->next;
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        if (t->next == NULL) {
            fprintf(stderr, "Erro de alocacao de memoria para linha\n");
            return;
        }
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

/**
 * @brief Busca um simbolo em todos os escopos (do atual ate o global)
 * @param name Nome do identificador
 * @return Ponteiro para o bucket ou NULL se nao encontrado
 */
BucketList st_lookup(char* name) {
    ScopeList scope = scopeStack;
    int h = hash(name);
    while (scope != NULL) {
        BucketList l = scope->hashTable[h];
        while ((l != NULL) && (strcmp(name, l->name) != 0))
            l = l->next;
        if (l != NULL)
            return l;
        scope = scope->parent;
    }
    return NULL;
}

/**
 * @brief Busca um simbolo apenas no escopo atual
 * @param name Nome do identificador
 * @return Ponteiro para o bucket ou NULL se nao encontrado
 */
BucketList st_lookup_top(char* name) {
    int h;
    BucketList l;
    if (scopeStack == NULL)
        return NULL;
    h = hash(name);
    l = scopeStack->hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    return l;
}

/**
 * @brief Imprime a tabela de simbolos completa
 */
void printSymTab(void) {
    ScopeList scope = allScopes;
    int i;
    while (scope != NULL) {
        printf("\nEscopo: %s (nivel %d)\n", scope->scopeName, scope->nestedLevel); // nivel pra funcs externas e shadowing
        printf("%-15s %-10s %-10s %-15s\n", "Nome", "Tipo", "MemLoc", "Linhas");
        printf("*******************************************************\n");
        for (i = 0; i < SIZE; i++) {
            if (scope->hashTable[i] != NULL) {
                BucketList l = scope->hashTable[i];
                while (l != NULL) {
                    LineList t = l->lines;
                    char* typeStr;
                    switch (l->type) {
                        case Integer:
                            typeStr = "int";
                            break;
                        case Void:
                            typeStr = "void";
                            break;
                        case IntegerArray:
                            typeStr = "int[]";
                            break;
                        case Boolean_:
                            typeStr = "boolean";
                            break;
                        default:
                            typeStr = "unknown";
                            break;
                    }
                    printf("%-15s %-10s %-10d ", l->name, typeStr, l->memloc);
                    while (t != NULL) {
                        printf("%d ", t->lineno);
                        t = t->next;
                    }
                    printf("\n");
                    l = l->next;
                }
            }
        }
        scope = scope->next;
    }
    printf("\n*******************************************************\n\n");
}

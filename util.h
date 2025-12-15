/**
 * @file util.h
 * @brief Funcoes auxiliares para manipulacao da AST
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"

/**
 * @brief Cria um novo no de statement na AST
 * @param kind Tipo do statement
 * @return Ponteiro para o novo no criado
 */
TreeNode* newStmtNode(StmtKind kind);

/**
 * @brief Cria um novo no de expressao na AST
 * @param kind Tipo da expressao
 * @return Ponteiro para o novo no criado
 */
TreeNode* newExpNode(ExpKind kind);

/**
 * @brief Cria um novo no de declaracao na AST
 * @param kind Tipo da declaracao
 * @return Ponteiro para o novo no criado
 */
TreeNode* newDeclNode(DeclKind kind);

/**
 * @brief Copia uma string para memoria alocada dinamicamente
 * @param s String a ser copiada
 * @return Ponteiro para a string copiada
 */
char* copyString(char* s);

/**
 * @brief Imprime a AST de forma identada
 * @param tree Raiz da arvore a ser impressa
 */
void printTree(TreeNode* tree);

#endif

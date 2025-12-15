/**
 * @file analyze.h
 * @brief Interface do analisador semantico
 */

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "globals.h"

/**
 * @brief Constroi a tabela de simbolos atraves de travessia da AST
 * @param syntaxTree Raiz da arvore sintatica
 */
void buildSymtab(TreeNode* syntaxTree);

/**
 * @brief Realiza verificacao de tipos atraves de travessia da AST
 * @param syntaxTree Raiz da arvore sintatica
 */
void typeCheck(TreeNode* syntaxTree);

#endif

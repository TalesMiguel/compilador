/**
 * @file cgen.h
 * @brief Interface do gerador de codigo intermediario
 */

#ifndef _CGEN_H_
#define _CGEN_H_

#include "globals.h"

/**
 * @brief Gera codigo intermediario de tres enderecos a partir da AST
 * @param syntaxTree Raiz da arvore sintatica
 */
void codeGen(TreeNode* syntaxTree);

#endif

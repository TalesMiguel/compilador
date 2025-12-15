/**
 * @file cgen.c
 * @brief Implementacao do gerador de codigo intermediario de tres enderecos
 */

#include "globals.h"
#include "cgen.h"
#include "cminus.tab.h"
#include <stdlib.h>

static int tempCount = 0;
static int labelCount = 0;

static char* cGenExp(TreeNode* tree);
static void cGenStmt(TreeNode* tree);
static void cGenExpStmt(TreeNode* tree);

/**
 * @brief Gera um novo nome de variavel temporaria
 * @return Nome da variavel temporaria
 */
static char* newTemp(void) {
    char* tempName = (char*)malloc(10 * sizeof(char));
    sprintf(tempName, "t%d", tempCount++);
    return tempName;
}

/**
 * @brief Gera um novo label
 * @return Nome do label
 */
static char* newLabel(void) {
    char* labelName = (char*)malloc(10 * sizeof(char));
    sprintf(labelName, "L%d", labelCount++);
    return labelName;
}

/**
 * @brief Gera codigo para expressoes
 * @param tree No da arvore
 * @return Nome da variavel temporaria com o resultado
 */
static char* cGenExp(TreeNode* tree) {
    char* temp;
    char* left;
    char* right;
    char* opStr;
    if (tree == NULL)
        return NULL;
    switch (tree->kind.exp) {
        case ConstK: // constante numerica
            temp = newTemp();
            printf("%s = %d\n", temp, tree->attr.val);
            return temp;
        case IdK: // identificador
            if (tree->child[0] != NULL) {
                char* index = cGenExp(tree->child[0]);
                temp = newTemp();
                printf("%s = %s[%s]\n", temp, tree->attr.name, index);
                return temp;
            }
            return tree->attr.name;
        case OpK: // operador
            left = cGenExp(tree->child[0]);
            right = cGenExp(tree->child[1]);
            temp = newTemp();
            switch (tree->attr.op) {
                case MAIS:
                    opStr = "+";
                    break;
                case MENOS:
                    opStr = "-";
                    break;
                case VEZES:
                    opStr = "*";
                    break;
                case SOBRE:
                    opStr = "/";
                    break;
                case MENOR:
                    opStr = "<";
                    break;
                case MENORIGUAL:
                    opStr = "<=";
                    break;
                case MAIOR:
                    opStr = ">";
                    break;
                case MAIORIGUAL:
                    opStr = ">=";
                    break;
                case IGUAL:
                    opStr = "==";
                    break;
                case DIFERENTE:
                    opStr = "!=";
                    break;
                default:
                    opStr = "?";
                    break;
            }
            printf("%s = %s %s %s\n", temp, left, opStr, right);
            return temp;
        case CallK: // chamada de funcao
            {
                TreeNode* arg = tree->child[0];
                int argCount = 0;
                while (arg != NULL) {
                    char* argTemp = cGenExp(arg);
                    printf("param %s\n", argTemp);
                    argCount++;
                    arg = arg->sibling;
                }
                temp = newTemp();
                printf("%s = call %s, %d\n", temp, tree->attr.name, argCount);
                return temp;
            }
        default:
            return NULL;
    }
}

/**
 * @brief Gera codigo para statements
 * @param tree No da arvore
 */
static void cGenStmt(TreeNode* tree) {
    char* test;
    char* labelFalse;
    char* labelEnd;
    char* labelStart;
    char* value;
    if (tree == NULL)
        return;
    switch (tree->kind.stmt) {
        case AssignK: // atribuicao
            if (tree->child[0] != NULL && tree->child[1] != NULL) {
                value = cGenExp(tree->child[1]);
                if (tree->child[0]->child[0] != NULL) {
                    char* index = cGenExp(tree->child[0]->child[0]);
                    printf("%s[%s] = %s\n", tree->child[0]->attr.name, index, value);
                } else {
                    printf("%s = %s\n", tree->child[0]->attr.name, value);
                }
            }
            break;
        case IfK: // if/if-else
            test = cGenExp(tree->child[0]);
            labelFalse = newLabel();
            labelEnd = newLabel();
            printf("if_false %s goto %s\n", test, labelFalse);
            cGenStmt(tree->child[1]);
            if (tree->child[2] != NULL) {
                printf("goto %s\n", labelEnd);
                printf("%s:\n", labelFalse);
                cGenStmt(tree->child[2]);
                printf("%s:\n", labelEnd);
            } else {
                printf("%s:\n", labelFalse);
            }
            break;
        case WhileK: // while loop
            labelStart = newLabel();
            labelEnd = newLabel();
            printf("%s:\n", labelStart);
            test = cGenExp(tree->child[0]);
            printf("if_false %s goto %s\n", test, labelEnd);
            cGenStmt(tree->child[1]);
            printf("goto %s\n", labelStart);
            printf("%s:\n", labelEnd);
            break;
        case ReturnK: // return
            if (tree->child[0] != NULL) {
                value = cGenExp(tree->child[0]);
                printf("return %s\n", value);
            } else {
                printf("return\n");
            }
            break;
        case CompoundK: // bloco composto { ... }
            if (tree->child[1] != NULL) {
                TreeNode* stmt = tree->child[1];
                while (stmt != NULL) {
                    if (stmt->nodekind == StmtK) {
                        cGenStmt(stmt);
                    } else if (stmt->nodekind == ExpK) {
                        cGenExpStmt(stmt);
                    }
                    stmt = stmt->sibling;
                }
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Gera codigo para declaracoes
 * @param tree No da arvore
 */
static void cGenDecl(TreeNode* tree) {
    if (tree == NULL)
        return;
    switch (tree->kind.decl) {
        case FunK: // funcao
            printf("\nfunc %s:\n", tree->attr.name);
            if (tree->child[0] != NULL) {
                TreeNode* param = tree->child[0];
                while (param != NULL) {
                    printf("param %s\n", param->attr.name);
                    param = param->sibling;
                }
            }
            if (tree->child[1] != NULL) {
                cGenStmt(tree->child[1]);
            }
            printf("endfunc\n");
            break;
        case VarK: // variavel
            break;
        case ArrayK: // array
            if (tree->child[0] != NULL) {
                printf("array %s[%d]\n", tree->attr.name, tree->child[0]->attr.val);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Gera codigo para expressoes usadas como statements
 * @param tree No da arvore
 */
static void cGenExpStmt(TreeNode* tree) {
    if (tree == NULL)
        return;
    if (tree->kind.exp == CallK) {
        TreeNode* arg = tree->child[0];
        int argCount = 0;
        while (arg != NULL) {
            char* argTemp = cGenExp(arg);
            printf("param %s\n", argTemp);
            argCount++;
            arg = arg->sibling;
        }
        printf("call %s, %d\n", tree->attr.name, argCount);
    }
}

/**
 * @brief Percorre a AST gerando codigo intermediario
 * @param tree No da arvore
 */
static void cGenTree(TreeNode* tree) {
    if (tree != NULL) {
        switch (tree->nodekind) {
            case StmtK: // statement
                cGenStmt(tree);
                break;
            case ExpK: // expressao
                cGenExpStmt(tree);
                break;
            case DeclK: // declaracao
                cGenDecl(tree);
                break;
            default:
                break;
        }
        cGenTree(tree->sibling); // processa irmaos
    }
}

/**
 * @brief Gera codigo intermediario de tres enderecos a partir da AST
 * @param syntaxTree Raiz da arvore sintatica
 */
void codeGen(TreeNode* syntaxTree) {
    printf("\n*** CODIGO INTERMEDIARIO (3 ENDERECOS) ***\n\n");
    cGenTree(syntaxTree);
    printf("\n******************************************\n\n");
}

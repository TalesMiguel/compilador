/**
 * @file util.c
 * @brief Implementacao das funcoes auxiliares para manipulacao da AST
 */

#include "globals.h"
#include "util.h"
#include "cminus.tab.h"

/**
 * @brief Cria um novo no de statement na AST
 * @param kind Tipo do statement
 * @return Ponteiro para o novo no criado
 */
TreeNode* newStmtNode(StmtKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria na linha %d\n", lineno);
    } else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

/**
 * @brief Cria um novo no de expressao na AST
 * @param kind Tipo da expressao
 * @return Ponteiro para o novo no criado
 */
TreeNode* newExpNode(ExpKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria na linha %d\n", lineno);
    } else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

/**
 * @brief Cria um novo no de declaracao na AST
 * @param kind Tipo da declaracao
 * @return Ponteiro para o novo no criado
 */
TreeNode* newDeclNode(DeclKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria na linha %d\n", lineno);
    } else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DeclK;
        t->kind.decl = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

/**
 * @brief Copia uma string para memoria alocada dinamicamente
 * @param s String a ser copiada
 * @return Ponteiro para a string copiada
 */
char* copyString(char* s) {
    int n;
    char* t;
    if (s == NULL)
        return NULL;
    n = strlen(s) + 1; // +1 para caractere nulo
    t = (char*)malloc(n);
    if (t == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria na linha %d\n", lineno);
    } else {
        strcpy(t, s);
    }
    return t;
}

static int indentno = 0;

#define INDENT indentno += 4
#define UNINDENT indentno -= 4

/**
 * @brief Printa espacos para indentacao
 */
static void printSpaces(void) {
    int i;
    for (i = 0; i < indentno; i++)
        printf(" ");
}

/**
 * @brief Printa a AST indentada
 * @param tree Raiz da arvore a ser impressa
 */
void printTree(TreeNode* tree) {
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
                case IfK:
                    printf("If\n");
                    break;
                case WhileK:
                    printf("While\n");
                    break;
                case AssignK:
                    printf("Assign\n");
                    break;
                case ReturnK:
                    printf("Return\n");
                    break;
                case CompoundK:
                    printf("Compound Statement\n");
                    break;
                default:
                    printf("Erro: no de statement desconhecido\n");
                    break;
            }
        } else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
                case OpK:
                    printf("Op: ");
                    switch (tree->attr.op) {
                        case MAIS:
                            printf("+");
                            break;
                        case MENOS:
                            printf("-");
                            break;
                        case VEZES:
                            printf("*");
                            break;
                        case SOBRE:
                            printf("/");
                            break;
                        case MENOR:
                            printf("<");
                            break;
                        case MENORIGUAL:
                            printf("<=");
                            break;
                        case MAIOR:
                            printf(">");
                            break;
                        case MAIORIGUAL:
                            printf(">=");
                            break;
                        case IGUAL:
                            printf("==");
                            break;
                        case DIFERENTE:
                            printf("!=");
                            break;
                        default:
                            printf("?");
                            break;
                    }
                    printf("\n");
                    break;
                case ConstK:
                    printf("Const: %d\n", tree->attr.val);
                    break;
                case IdK:
                    printf("Id: %s\n", tree->attr.name);
                    break;
                case CallK:
                    printf("Call: %s\n", tree->attr.name);
                    break;
                default:
                    printf("Erro: no de expressao desconhecido\n");
                    break;
            }
        } else if (tree->nodekind == DeclK) {
            switch (tree->kind.decl) {
                case VarK:
                    printf("Var Declaration: %s", tree->attr.name);
                    if (tree->type == Integer)
                        printf(" (int)\n");
                    else if (tree->type == Void)
                        printf(" (void)\n");
                    else
                        printf("\n");
                    break;
                case ArrayK:
                    printf("Array Declaration: %s", tree->attr.name);
                    if (tree->child[0] != NULL)
                        printf("[%d]", tree->child[0]->attr.val);
                    printf("\n");
                    break;
                case FunK:
                    printf("Function Declaration: %s", tree->attr.name);
                    if (tree->type == Integer)
                        printf(" returns int\n");
                    else if (tree->type == Void)
                        printf(" returns void\n");
                    else
                        printf("\n");
                    break;
                case ParamK:
                    printf("Parameter: %s", tree->attr.name);
                    if (tree->type == Integer)
                        printf(" (int)\n");
                    else if (tree->type == IntegerArray)
                        printf(" (int[])\n");
                    else if (tree->type == Void)
                        printf(" (void)\n");
                    else
                        printf("\n");
                    break;
                default:
                    printf("Erro: no de declaracao desconhecido\n");
                    break;
            }
        } else {
            printf("Erro: tipo de no desconhecido\n");
        }
        for (i = 0; i < MAXCHILDREN; i++)
            printTree(tree->child[i]); // Recursao pros filhos
        tree = tree->sibling;
    }
    UNINDENT;
}

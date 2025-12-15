/**
 * @file analyze.c
 * @brief Implementacao do analisador semantico
 */

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

static int globalMemLoc = 0;  // Contador para variaveis globais
static int localMemLoc = 0;   // Contador para variaveis locais


/**
 * @brief Travessia da AST para construir a tabela de simbolos
 * @param t No da arvore
 * @param preProc Funcao executada antes de processar filhos
 * @param postProc Funcao executada apos processar filhos
 */
static void traverse(TreeNode* t,
                    void (*preProc)(TreeNode*),
                    void (*postProc)(TreeNode*)) {
    if (t != NULL) {
        preProc(t);
        int i;
        for (i = 0; i < MAXCHILDREN; i++)
            traverse(t->child[i], preProc, postProc);
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

/**
 * @brief Funcao vazia para travessia
 * @param t No da arvore
 */
static void nullProc(TreeNode* t) {
    if (t == NULL)
        return;
}

/**
 * @brief Insere identificador na tabela de simbolos
 * @param t No da arvore
 */
static void insertNode(TreeNode* t) {
    BucketList l;
    switch (t->nodekind) {
        case DeclK:
            switch (t->kind.decl) {
                case VarK:
                case ArrayK:
                    if (st_lookup_top(t->attr.name) != NULL) {
                        fprintf(stderr, "ERRO SEMANTICO: Variavel '%s' ja foi declarada neste escopo. Linha: %d\n",
                                t->attr.name, t->lineno);
                        Error = TRUE;
                    } else {
                        if (t->type == Void) {
                            fprintf(stderr, "ERRO SEMANTICO: Variavel '%s' declarada com tipo void. Linha: %d\n",
                                    t->attr.name, t->lineno);
                            Error = TRUE;
                        } else {
                            st_insert(t->attr.name, t->type, t->lineno, globalMemLoc++);
                        }
                    }
                    break;
                case FunK:
                    if (st_lookup(t->attr.name) != NULL) {
                        fprintf(stderr, "ERRO SEMANTICO: Funcao '%s' ja foi declarada. Linha: %d\n",
                                t->attr.name, t->lineno);
                        Error = TRUE;
                    } else {
                        st_insert(t->attr.name, t->type, t->lineno, globalMemLoc++);
                        st_push_scope(t->attr.name);
                        localMemLoc = 0;
                    }
                    break;
                case ParamK:
                    if (st_lookup_top(t->attr.name) != NULL) {
                        fprintf(stderr, "ERRO SEMANTICO: Parametro '%s' ja foi declarado nesta funcao. Linha: %d\n",
                                t->attr.name, t->lineno);
                        Error = TRUE;
                    } else {
                        st_insert(t->attr.name, t->type, t->lineno, localMemLoc++);
                    }
                    break;
                default:
                    break;
            }
            break;
        case StmtK:
            switch (t->kind.stmt) {
                case CompoundK:
                    if (t->child[0] != NULL || t->child[1] != NULL) {
                    }
                    break;
                default:
                    break;
            }
            break;
        case ExpK:
            switch (t->kind.exp) {
                case IdK:
                case CallK:
                    l = st_lookup(t->attr.name);
                    if (l == NULL) {
                        fprintf(stderr, "ERRO SEMANTICO: Identificador '%s' nao foi declarado no escopo atual. Linha: %d\n",
                                t->attr.name, t->lineno);
                        Error = TRUE;
                    } else {
                        LineList ll = l->lines;
                        while (ll->next != NULL)
                            ll = ll->next;
                        ll->next = (LineList)malloc(sizeof(struct LineListRec));
                        ll->next->lineno = t->lineno;
                        ll->next->next = NULL;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Remove escopo apos processar funcao
 * @param t No da arvore
 */
static void afterInsertNode(TreeNode* t) {
    if (t->nodekind == DeclK && t->kind.decl == FunK) {
        st_pop_scope();
    }
}

/**
 * @brief Constroi a tabela de simbolos atraves de travessia da AST
 * @param syntaxTree Raiz da arvore sintatica
 */
void buildSymtab(TreeNode* syntaxTree) {
    st_push_scope("global");
    st_insert("input", Integer, 0, globalMemLoc++);
    st_insert("output", Void, 0, globalMemLoc++);
    traverse(syntaxTree, insertNode, afterInsertNode);
}

/**
 * @brief Define tipos dos nos antes de verificacao
 * @param t No da arvore
 */
static void setNodeTypes(TreeNode* t) {
    if (t == NULL)
        return;
    switch (t->nodekind) {
        case ExpK:
            switch (t->kind.exp) {
                case ConstK:
                    t->type = Integer;
                    break;
                case IdK:
                    {
                        BucketList l = st_lookup(t->attr.name);
                        if (l != NULL) {
                            t->type = l->type;
                            if (t->child[0] != NULL && l->type == IntegerArray) {
                                t->type = Integer;
                            }
                        }
                    }
                    break;
                case OpK:
                    t->type = Integer;
                    break;
                case CallK:
                    {
                        BucketList l = st_lookup(t->attr.name);
                        if (l != NULL) {
                            t->type = l->type;
                        } else {
                            t->type = Void;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Verifica tipos de um no
 * @param t No da arvore
 */
static void checkNode(TreeNode* t) {
    switch (t->nodekind) {
        case StmtK:
            switch (t->kind.stmt) {
                case AssignK:
                    if (t->child[0] != NULL && t->child[1] != NULL) {
                        if (t->child[0]->type == IntegerArray) {
                            if (t->child[0]->child[0] == NULL) {
                                fprintf(stderr, "ERRO SEMANTICO: Atribuicao invalida para array '%s' sem indice. Linha: %d\n",
                                        t->child[0]->attr.name, t->lineno);
                                Error = TRUE;
                            }
                        }
                        if (t->child[1]->type == Void) {
                            fprintf(stderr, "ERRO SEMANTICO: Atribuicao invalida, expressao do lado direito retorna void. Linha: %d\n",
                                    t->lineno);
                            Error = TRUE;
                        }
                    }
                    break;
                case ReturnK:
                    break;
                default:
                    break;
            }
            break;
        case ExpK:
            switch (t->kind.exp) {
                case OpK:
                    if (t->child[0] != NULL && t->child[0]->type == Void) {
                        fprintf(stderr, "ERRO SEMANTICO: Operando esquerdo do operador tem tipo void. Linha: %d\n",
                                t->lineno);
                        Error = TRUE;
                    }
                    if (t->child[1] != NULL && t->child[1]->type == Void) {
                        fprintf(stderr, "ERRO SEMANTICO: Operando direito do operador tem tipo void. Linha: %d\n",
                                t->lineno);
                        Error = TRUE;
                    }
                    t->type = Integer;
                    break;
                case ConstK:
                    t->type = Integer;
                    break;
                case IdK:
                    {
                        BucketList l = st_lookup(t->attr.name);
                        if (l != NULL) {
                            t->type = l->type;
                            if (t->child[0] != NULL) {
                                if (l->type != IntegerArray) {
                                    fprintf(stderr, "ERRO SEMANTICO: Identificador '%s' nao e um array. Linha: %d\n",
                                            t->attr.name, t->lineno);
                                    Error = TRUE;
                                } else {
                                    t->type = Integer;
                                }
                            }
                        }
                    }
                    break;
                case CallK:
                    {
                        BucketList l = st_lookup(t->attr.name);
                        if (l != NULL) {
                            t->type = l->type;
                        } else {
                            t->type = Void;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case DeclK:
            switch (t->kind.decl) {
                case FunK:
                    {
                        if (strcmp(t->attr.name, "main") != 0 &&
                            strcmp(t->attr.name, "input") != 0 &&
                            strcmp(t->attr.name, "output") != 0) {
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Realiza verificacao de tipos atraves de travessia da AST
 * @param syntaxTree Raiz da arvore sintatica
 */
void typeCheck(TreeNode* syntaxTree) {
    traverse(syntaxTree, setNodeTypes, nullProc);
    traverse(syntaxTree, checkNode, nullProc);
    BucketList mainFunc = st_lookup("main");
    if (mainFunc == NULL) {
        fprintf(stderr, "ERRO SEMANTICO: Funcao 'main' nao foi declarada no programa.\n");
        Error = TRUE;
    }
}

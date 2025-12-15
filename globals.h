/**
 * @file globals.h
 * @brief Definicoes globais e estruturas de dados para o compilador C-
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXCHILDREN 3

typedef int Boolean;
typedef int TokenType;

extern int lineno;

// Categorias de nos da AST
typedef enum {
    StmtK,     // Statement (comando/instrucao)
    ExpK,      // Expression (expressao)
    DeclK      // Declaration (declaracao)
} NodeKind;

// Tipos de statements (comandos)
typedef enum {
    IfK,         // if/if-else
    WhileK,      // while loop
    ReturnK,     // return
    CompoundK,   // bloco composto { ... }
    AssignK      // atribuicao =
} StmtKind;

// Tipos de expressoes
typedef enum {
    OpK,       // operador (+, -, *, /, <, >, etc)
    ConstK,    // constante numerica
    IdK,       // identificador (variavel/array)
    CallK      // chamada de funcao
} ExpKind;

// Tipos de declaracoes
typedef enum {
    VarK,      // variavel
    FunK,      // funcao
    ParamK,    // parametro
    ArrayK     // array
} DeclKind;

// Tipos de dados da linguagem C- segundo o Louden
typedef enum {
    Void,          // tipo void (funcoes sem retorno)
    Integer,       // tipo int
    IntegerArray,  // tipo int[] (array)
    Boolean_       // tipo boolean (interno)
} ExpType;

// Estrutura de um no da (Arvore Sintatica Abstrata)
typedef struct treeNode {
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;
    int lineno;
    NodeKind nodekind;

    // Tipo especifico do no
    union {
        StmtKind stmt;
        ExpKind exp;
        DeclKind decl;
    } kind;

    // Atributos do no
    union {
        TokenType op;
        int val;
        char* name;
    } attr;

    ExpType type;
} TreeNode;

extern FILE* source;
extern FILE* listing;
extern FILE* code;

extern int EchoSource;
extern int TraceScan;
extern int TraceParse;
extern int TraceAnalyze;
extern int TraceCode;
extern int Error;

#endif

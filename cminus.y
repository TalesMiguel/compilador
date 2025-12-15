/**
 * @file cminus.y
 * @brief Analisador sintatico para a linguagem C-
 */

%{
#include "globals.h"
#include "util.h"

TreeNode* savedTree;

extern int lineno;
extern int yylex(void);
void yyerror(const char* message);
%}

%union {
    TreeNode* node;
    int val;
    char* name;
    TokenType token;
}

%token <name> ID
%token <val> NUM
%token ELSE IF INT RETURN VOID WHILE
%token ATRIBUICAO IGUAL MENOR MENORIGUAL MAIOR MAIORIGUAL DIFERENTE
%token MAIS MENOS VEZES SOBRE
%token LPARENTESES RPARENTESES LCOLCHETE RCOLCHETE LCHAVE RCHAVE
%token PONTOEVIRGULA VIRGULA
%token ERROR ENDFILE

%type <node> programa lista_declaracoes declaracao var_declaracao func_declaracao
%type <node> params param_lista param composto_decl local_declaracoes
%type <node> statement_lista statement expressao_decl selecao_decl iteracao_decl
%type <node> retorno_decl expressao var expressao_simples relacional termo fator
%type <node> ativacao args arg_lista
%type <token> relop soma mult tipo_especificador

%right ATRIBUICAO
%left IGUAL DIFERENTE
%left MENOR MENORIGUAL MAIOR MAIORIGUAL
%left MAIS MENOS
%left VEZES SOBRE

%%

programa
    : lista_declaracoes
        { savedTree = $1; }
    ;

lista_declaracoes
    : lista_declaracoes declaracao
        {
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | declaracao
        { $$ = $1; }
    ;

declaracao
    : var_declaracao
        { $$ = $1; }
    | func_declaracao
        { $$ = $1; }
    ;

var_declaracao
    : tipo_especificador ID PONTOEVIRGULA
        {
            $$ = newDeclNode(VarK);
            $$->attr.name = $2;
            $$->type = ($1 == INT) ? Integer : Void;
        }
    | tipo_especificador ID LCOLCHETE NUM RCOLCHETE PONTOEVIRGULA
        {
            $$ = newDeclNode(ArrayK);
            $$->attr.name = $2;
            $$->type = IntegerArray;
            $$->child[0] = newExpNode(ConstK);
            $$->child[0]->attr.val = $4;
        }
    ;

tipo_especificador
    : INT
        { $$ = INT; }
    | VOID
        { $$ = VOID; }
    ;

func_declaracao
    : tipo_especificador ID LPARENTESES params RPARENTESES composto_decl
        {
            $$ = newDeclNode(FunK);
            $$->attr.name = $2;
            $$->type = ($1 == INT) ? Integer : Void;
            $$->child[0] = $4;
            $$->child[1] = $6;
        }
    ;

params
    : param_lista
        { $$ = $1; }
    | VOID
        { $$ = NULL; }
    ;

param_lista
    : param_lista VIRGULA param
        {
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $3;
                $$ = $1;
            } else {
                $$ = $3;
            }
        }
    | param
        { $$ = $1; }
    ;

param
    : tipo_especificador ID
        {
            $$ = newDeclNode(ParamK);
            $$->attr.name = $2;
            $$->type = ($1 == INT) ? Integer : Void;
        }
    | tipo_especificador ID LCOLCHETE RCOLCHETE
        {
            $$ = newDeclNode(ParamK);
            $$->attr.name = $2;
            $$->type = IntegerArray;
        }
    ;

// compound-stmt: Segue estritamente a gramatica BNF do Apendice A do Louden (pag. 490+)
composto_decl
    : LCHAVE local_declaracoes statement_lista RCHAVE
        {
            $$ = newStmtNode(CompoundK);
            $$->child[0] = $2;
            $$->child[1] = $3;
        }
    ;

local_declaracoes
    : local_declaracoes var_declaracao
        {
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | /* empty */
        { $$ = NULL; }
    ;



statement_lista
    : statement_lista statement
        {
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | /* empty */
        { $$ = NULL; }
    ;

statement
    : expressao_decl
        { $$ = $1; }
    | composto_decl
        { $$ = $1; }
    | selecao_decl
        { $$ = $1; }
    | iteracao_decl
        { $$ = $1; }
    | retorno_decl
        { $$ = $1; }
    ;

expressao_decl
    : expressao PONTOEVIRGULA
        { $$ = $1; }
    | PONTOEVIRGULA
        { $$ = NULL; }
    ;

selecao_decl
    : IF LPARENTESES expressao RPARENTESES statement
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
        }
    | IF LPARENTESES expressao RPARENTESES statement ELSE statement
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = $7;
        }
    ;

iteracao_decl
    : WHILE LPARENTESES expressao RPARENTESES statement
        {
            $$ = newStmtNode(WhileK);
            $$->child[0] = $3;
            $$->child[1] = $5;
        }
    ;

retorno_decl
    : RETURN PONTOEVIRGULA
        {
            $$ = newStmtNode(ReturnK);
        }
    | RETURN expressao PONTOEVIRGULA
        {
            $$ = newStmtNode(ReturnK);
            $$->child[0] = $2;
        }
    ;

expressao
    : var ATRIBUICAO expressao
        {
            $$ = newStmtNode(AssignK);
            $$->child[0] = $1;
            $$->child[1] = $3;
        }
    | expressao_simples
        { $$ = $1; }
    ;

var
    : ID
        {
            $$ = newExpNode(IdK);
            $$->attr.name = $1;
        }
    | ID LCOLCHETE expressao RCOLCHETE
        {
            $$ = newExpNode(IdK);
            $$->attr.name = $1;
            $$->child[0] = $3;
        }
    ;

expressao_simples
    : expressao_simples relop relacional
        {
            $$ = newExpNode(OpK);
            $$->attr.op = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
        }
    | relacional
        { $$ = $1; }
    ;

relop
    : MENORIGUAL
        { $$ = MENORIGUAL; }
    | MENOR
        { $$ = MENOR; }
    | MAIOR
        { $$ = MAIOR; }
    | MAIORIGUAL
        { $$ = MAIORIGUAL; }
    | IGUAL
        { $$ = IGUAL; }
    | DIFERENTE
        { $$ = DIFERENTE; }
    ;

relacional
    : relacional soma termo
        {
            $$ = newExpNode(OpK);
            $$->attr.op = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
        }
    | termo
        { $$ = $1; }
    ;

soma
    : MAIS
        { $$ = MAIS; }
    | MENOS
        { $$ = MENOS; }
    ;

termo
    : termo mult fator
        {
            $$ = newExpNode(OpK);
            $$->attr.op = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
        }
    | fator
        { $$ = $1; }
    ;

mult
    : VEZES
        { $$ = VEZES; }
    | SOBRE
        { $$ = SOBRE; }
    ;

fator
    : LPARENTESES expressao RPARENTESES
        { $$ = $2; }
    | var
        { $$ = $1; }
    | ativacao
        { $$ = $1; }
    | NUM
        {
            $$ = newExpNode(ConstK);
            $$->attr.val = $1;
        }
    ;

ativacao
    : ID LPARENTESES args RPARENTESES
        {
            $$ = newExpNode(CallK);
            $$->attr.name = $1;
            $$->child[0] = $3;
        }
    ;

args
    : arg_lista
        { $$ = $1; }
    | /* empty */
        { $$ = NULL; }
    ;

arg_lista
    : arg_lista VIRGULA expressao
        {
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $3;
                $$ = $1;
            } else {
                $$ = $3;
            }
        }
    | expressao
        { $$ = $1; }
    ;

%%

/**
 * @brief Funcao de tratamento de erros sintaticos
 * @param message Mensagem de erro
 */
void yyerror(const char* message) {
    fprintf(stderr, "ERRO SINTATICO: %s LINHA: %d\n", message, lineno);
    Error = TRUE;
}

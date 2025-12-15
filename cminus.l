/**
 * @file cminus.l
 * @brief Analisador lexico para a linguagem C-
 */

%{
#include "globals.h"
#include "cminus.tab.h"

int lineno = 1;
%}

digito          [0-9]
numero          {digito}+
letra           [a-zA-Z]
identificador   {letra}+
espaco          [ \t]+

%%

"else"              { return ELSE; }
"if"                { return IF; }
"int"               { return INT; }
"return"            { return RETURN; }
"void"              { return VOID; }
"while"             { return WHILE; }

"+"                 { return MAIS; }
"-"                 { return MENOS; }
"*"                 { return VEZES; }
"/"                 { return SOBRE; }
"<"                 { return MENOR; }
"<="                { return MENORIGUAL; }
">"                 { return MAIOR; }
">="                { return MAIORIGUAL; }
"=="                { return IGUAL; }
"!="                { return DIFERENTE; }
"="                 { return ATRIBUICAO; }
";"                 { return PONTOEVIRGULA; }
","                 { return VIRGULA; }
"("                 { return LPARENTESES; }
")"                 { return RPARENTESES; }
"["                 { return LCOLCHETE; }
"]"                 { return RCOLCHETE; }
"{"                 { return LCHAVE; }
"}"                 { return RCHAVE; }

{numero}            { yylval.val = atoi(yytext); return NUM; }
{identificador}     { yylval.name = strdup(yytext); return ID; }

{espaco}            { }

\n                  { lineno++; }

"/*"                {
                        char c;
                        char prev = '\0';
                        int comment_start = lineno;
                        while ((c = input()) != EOF) {
                            if (c == '\n') {
                                lineno++;
                            }
                            if (prev == '*' && c == '/') {
                                break;
                            }
                            prev = c;
                        }
                        if (c == EOF) {
                            fprintf(stderr, "ERRO LEXICO: '/*' LINHA: %d\n", comment_start);
                            return ERROR;
                        }
                    }

.                   {
                        fprintf(stderr, "ERRO LEXICO: '%s' LINHA: %d\n", yytext, lineno);
                        return ERROR;
                    }

%%

/**
 * @brief Funcao chamada ao fim do arquivo
 * @return 1 sempre
 */
int yywrap(void) {
    return 1;
}

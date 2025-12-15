/**
 * @file main.c
 * @brief Programa principal do compilador C-
 */

#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "analyze.h"
#include "cgen.h"

FILE* source;
FILE* listing;
int Error = FALSE;

extern int yyparse(void);
extern TreeNode* savedTree;
extern FILE* yyin;

/**
 * @brief Funcao principal do compilador
 * @param argc Numero de argumentos
 * @param argv Vetor de argumentos
 * @return 0 se sucesso, 1 se erro
 */
int main(int argc, char* argv[]) {
    TreeNode* syntaxTree;
    char* fileName;
    
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.cm>\n", argv[0]);
        return 1;
    }
    
    fileName = argv[1];
    source = fopen(fileName, "r");
    if (source == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'\n", fileName);
        return 1;
    }
    
    listing = stdout;
    yyin = source;
    
    fprintf(listing, "Arquivo de entrada: %s\n\n", fileName);
    
    yyparse();
    syntaxTree = savedTree;
    fclose(source);
    
    if (Error) {
        fprintf(listing, "\nErros encontrados durante a analise. Compilacao abortada.\n");
        return 1;
    }
    
    if (syntaxTree == NULL) {
        fprintf(listing, "\nErro: arvore sintatica nao foi construida.\n");
        return 1;
    }
    
    fprintf(listing, "\n******** ARVORE SINTATICA ABSTRATA ********\n\n");
    printTree(syntaxTree);
    
    fprintf(listing, "\n******** ANALISE SEMANTICA ********\n\n");
    fprintf(listing, "Construindo tabela de simbolos...\n");
    buildSymtab(syntaxTree);
    
    if (Error) {
        fprintf(listing, "\nErros semanticos encontrados. Compilacao abortada.\n");
        return 1;
    }
    
    fprintf(listing, "\nVerificacao de tipos...\n");
    typeCheck(syntaxTree);
    
    if (Error) {
        fprintf(listing, "\nErros de tipo encontrados. Compilacao abortada.\n");
        return 1;
    }
    
    fprintf(listing, "\n******** TABELA DE SIMBOLOS ********\n\n");
    printSymTab();
    st_pop_scope();
    
    fprintf(listing, "\n******** GERACAO DE CODIGO ********\n");
    codeGen(syntaxTree);
    
    fprintf(listing, "\nCompilacao concluida com sucesso!\n\n");
    
    return 0;
}

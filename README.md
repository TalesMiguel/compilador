# Compilador C-

Compilador para a linguagem C- (C-Minus) baseado no livro "Compiler Construction: Principles and Practice" de Kenneth C. Louden.

## Componentes

O compilador implementa:
- Scanner (análise léxica) com Flex
- Parser (análise sintática) com Bison
- Construção de Árvore Sintática Abstrata (AST)
- Tabela de Símbolos com pilha de escopos
- Análise Semântica (verificação de tipos)
- Geração de Código Intermediário (três endereços)

## Requisitos

- GCC
- Flex
- Bison
- Make

### Instalação (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install gcc flex bison make
```

## Compilação

```bash
make
```

Para limpar arquivos gerados:

```bash
make clean
```

## Execução

```bash
./cminus <arquivo.cm>
```

### Exemplo

```bash
./cminus teste.cm
```

## Saídas

O compilador produz três saídas principais:

1. **Árvore Sintática Abstrata (AST)**: Estrutura hierárquica do programa
2. **Tabela de Símbolos**: Símbolos organizados por escopo com tipos e localização
3. **Código Intermediário**: Código de três endereços sem otimizações

## Estrutura do Projeto

```
├── Makefile                 # Sistema de build
├── globals.h                # Definições globais
├── cminus.l                 # Scanner (Flex)
├── cminus.y                 # Parser (Bison)
├── util.h / util.c          # Funções auxiliares AST
├── symtab.h / symtab.c      # Tabela de símbolos
├── analyze.h / analyze.c    # Análise semântica
├── cgen.h / cgen.c          # Gerador de código
├── main.c                   # Programa principal
└── teste.cm                 # Arquivo de teste
```

## Referências

LOUDEN, K. C. *Compiler Construction: Principles and Practice*. Boston: PWS Publishing Company, 1997.

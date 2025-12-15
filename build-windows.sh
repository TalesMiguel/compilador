#!/bin/bash
# Script para compilar o projeto usando MinGW

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Compilador C- - Build para Windows ===${NC}\n"

if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo -e "${RED}Erro: MinGW-w64 não está instalado!${NC}"
    echo "Instale com: sudo apt-get install mingw-w64"
    exit 1
fi

# Verificar se flex e bison estão instalados
if ! command -v flex &> /dev/null; then
    echo -e "${RED}Erro: Flex não está instalado!${NC}"
    echo "Instale com: sudo apt-get install flex"
    exit 1
fi

if ! command -v bison &> /dev/null; then
    echo -e "${RED}Erro: Bison não está instalado!${NC}"
    echo "Instale com: sudo apt-get install bison"
    exit 1
fi

# clean
echo -e "${YELLOW}Limpando arquivos anteriores...${NC}"
make clean 2>/dev/null
rm -f cminus.exe *.o lex.yy.c cminus.tab.c cminus.tab.h

# Gerar parser e scanner
echo -e "${YELLOW}Gerando parser com Bison...${NC}"
bison -d cminus.y
if [ $? -ne 0 ]; then
    echo -e "${RED}Erro ao gerar parser!${NC}"
    exit 1
fi

echo -e "${YELLOW}Gerando scanner com Flex...${NC}"
flex cminus.l
if [ $? -ne 0 ]; then
    echo -e "${RED}Erro ao gerar scanner!${NC}"
    exit 1
fi

# Compilar para Windows usando MinGW
echo -e "${YELLOW}Compilando para Windows...${NC}"

CC=x86_64-w64-mingw32-gcc
CFLAGS="-Wall -Wno-unused-function -O2"

# Compilar cada arquivo objeto
echo "Compilando main.c..."
$CC $CFLAGS -c main.c -o main.o

echo "Compilando util.c..."
$CC $CFLAGS -c util.c -o util.o

echo "Compilando symtab.c..."
$CC $CFLAGS -c symtab.c -o symtab.o

echo "Compilando analyze.c..."
$CC $CFLAGS -c analyze.c -o analyze.o

echo "Compilando cgen.c..."
$CC $CFLAGS -c cgen.c -o cgen.o

echo "Compilando cminus.tab.c..."
$CC $CFLAGS -c cminus.tab.c -o cminus.tab.o

echo "Compilando lex.yy.c..."
$CC $CFLAGS -c lex.yy.c -o lex.yy.o

# Linkar tudo
echo -e "${YELLOW}Linkando executável...${NC}"
$CC $CFLAGS -o cminus.exe main.o util.o symtab.o analyze.o cgen.o cminus.tab.o lex.yy.o

if [ $? -eq 0 ]; then
    echo -e "\n${GREEN} Compilação concluída com sucesso!${NC}"
    echo -e "${GREEN} Executável criado: cminus.exe${NC}"
    ls -lh cminus.exe
    
    # Criar pacote para distribuição
    echo -e "\n${YELLOW}Criando pacote de distribuição...${NC}"
    mkdir -p dist
    cp cminus.exe dist/
    cp teste*.cm dist/ 2>/dev/null
    cp README.md dist/ 2>/dev/null
    
    echo -e "${GREEN} Arquivos copiados para o diretório 'dist/'${NC}"
    echo -e "\n${YELLOW}Para usar no Windows:${NC}"
    echo "1. Copie o diretório 'dist/' para o Windows"
    echo "2. Execute: cminus.exe arquivo.cm"
else
    echo -e "\n${RED}!!!!! Erro na compilação!!!!!${NC}"
    exit 1
fi

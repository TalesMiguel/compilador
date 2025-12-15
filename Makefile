# Makefile para o compilador C-

CC = gcc # compilador C
CFLAGS = -Wall -Wno-unused-function -g # ativa warnings de compilacao, desativa warnings de funcs inutilizadas, debug exec 
LEX = flex # gerador de analisador lexico
YACC = bison # gerador de analisador sintatico

TARGET = cminus
OBJS = main.o util.o symtab.o analyze.o cgen.o lex.yy.o cminus.tab.o

all: $(TARGET)

# gera o executavel cminus
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# compilacao dos modulos do compilador
main.o: main.c globals.h util.h symtab.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h cminus.tab.h
	$(CC) $(CFLAGS) -c util.c

symtab.o: symtab.c symtab.h globals.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h globals.h symtab.h
	$(CC) $(CFLAGS) -c analyze.c

cgen.o: cgen.c cgen.h globals.h cminus.tab.h
	$(CC) $(CFLAGS) -c cgen.c

# compilacao do parser gerado pelo Bison
cminus.tab.o: cminus.tab.c globals.h util.h
	$(CC) $(CFLAGS) -c cminus.tab.c

# compilacao do scanner gerado pelo Flex
lex.yy.o: lex.yy.c cminus.tab.h globals.h
	$(CC) $(CFLAGS) -c lex.yy.c

# geracao do parser: Bison processa cminus.y e gera cminus.tab.c e cminus.tab.h
cminus.tab.c cminus.tab.h: cminus.y
	$(YACC) -d cminus.y

# geracao do scanner: Flex processa cminus.l e gera lex.yy.c
lex.yy.c: cminus.l cminus.tab.h
	$(LEX) cminus.l

clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c cminus.tab.c cminus.tab.h

# Compilacao cruzada para Windows
windows: CC = x86_64-w64-mingw32-gcc
windows: CFLAGS = -Wall -Wno-unused-function -O2
windows: TARGET = cminus.exe
windows: clean-windows $(TARGET)
	@echo "Executavel Windows criado: $(TARGET)"
	@mkdir -p dist
	@cp $(TARGET) dist/
	@cp teste*.cm dist/ 2>/dev/null || true
	@echo "Arquivos copiados para dist/"

clean-windows:
	rm -f cminus.exe *.o lex.yy.c cminus.tab.c cminus.tab.h dist/cminus.exe

.PHONY: all clean

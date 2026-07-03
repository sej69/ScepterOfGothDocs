SHELL=/bin/sh
YFLAGS=-d
SRC=main.c subs.c yylex.c gram.c get_word.c
OBJ=main.o subs.o yylex.o gram.o get_word.o

all: lo

lo: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) /u/aek/lib/local.a -o $@

main.o subs.o yylex.o get_word.o: x.tab.h

x.tab.h: y.tab.h
	-cmp -s x.tab.h y.tab.h || cp y.tab.h x.tab.h

gram.o y.tab.h: gram.y
	yacc $(YFLAGS) gram.y
	mv y.tab.c gram.c
	$(CC) $(CFLAGS) -c gram.c

install:
	cp lo /u/aek/bin/lo
	chmod 755 /u/aek/bin/lo

clean:
	rm -f $(OBJ) [xy].tab.[ch]

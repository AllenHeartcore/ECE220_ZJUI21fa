EXE := 
CFLAGS := -Wall -g -ansi
TARGET := c220${EXE}
OFILES := ece220_lex.yy.o ece220_parse.tab.o ece220_symtab.o ece220_label.o \
	  mp11base.o mp11.o

ALL: ${TARGET}

${TARGET}: ${OFILES}
	gcc -g -o $@ ${OFILES}

ece220_lex.yy.c: ece220_lex.f ece220_parse.tab.h
	flex -o$@ -Pece220 $<

ece220_parse.tab.h: ece220_parse.y
	bison --debug --defines --name-prefix ece220 $<

ece220_parse.tab.c: ece220_parse.y
	bison --debug --defines --name-prefix ece220 $<

%.o: %.c
	gcc ${CFLAGS} -c -o $@ $<

clean::
	rm -f *.o *~ *.output

clear: clean
	rm -f ece220_lex.yy.c ece220_parse.tab.c ece220_parse.tab.h ${TARGET}

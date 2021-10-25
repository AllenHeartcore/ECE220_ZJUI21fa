CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -lpng -g
HEADERS := mp5.h Makefile
EXE := mp5
SOURCES := mp5.o main.o draw_tree.o

.ALL: tree_image.png

tree_image.png: ${EXE}
	./${EXE}

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} -o $@ $<

draw_tree.c: lab9
	./lab9 > draw_tree.c

lab9: lab9.o
	${CC} -o lab9 lab9.o ${LDFLAGS}

clean::
	rm -f ${SOURCES} mp5 lab9 tree_image.png draw_tree.c lab9.o


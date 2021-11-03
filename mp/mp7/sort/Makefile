CC := gcc
CFLAGS := -Wall -g
LDFLAGS := -g
HEADERS := sort.h
EXE := sort
SOURCES := sort.o
OBJ := sortMain.o

ALL: ${EXE}

sort.o: sort.c ${HEADERS} Makefile
	${CC} -c -Wall -o $@ $<

${EXE}: ${SOURCES} ${OBJ}
	$(CC) ${CFLAGS} ${SOURCES} ${OBJ} -o ${EXE} ${LDFLAGS}

clean::
	rm -f ${SOURCES} ${EXE}


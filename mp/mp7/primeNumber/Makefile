CC := gcc
CFLAGS := -Wall -g
LDFLAGS := -g
HEADERS := isPrime.h
EXE := primeNumber
SOURCES := primeNumber.o
OBJ := isPrime.o

ALL: ${EXE}

primeNumber.o: primeNumber.c ${HEADERS} Makefile
	${CC} -c -Wall -o $@ $<

${EXE}: ${SOURCES} ${OBJ}
	$(CC) ${CFLAGS} ${SOURCES} ${OBJ} -o ${EXE} ${LDFLAGS}

clean::
	rm -f ${SOURCES} ${EXE}


CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g
HEADERS := lab11.h Makefile
EXE := lab11
SOURCES := lab11.o lab11main.o

.ALL: ${EXE}

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} -o $@ $<

clean::
	rm -f ${SOURCES}

clear: clean
	rm -f ${EXE}


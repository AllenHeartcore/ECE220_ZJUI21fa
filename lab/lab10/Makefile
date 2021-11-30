CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g
HEADERS := lab10.h Makefile
EXE := lab10
SOURCES := lab10.o lab10main.o

.ALL: ${EXE}

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} -o $@ $<

clean::
	rm -f ${SOURCES}

clear: clean
	rm -f ${EXE}


CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g
HEADERS := lab13.h Makefile
EXE := lab13
SOURCES := lab13.o lab13main.o

.ALL: ${EXE}

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} -o $@ $<

clean::
	rm -f ${SOURCES} ${EXE}.exe.stackdump

clear: clean
	rm -f ${EXE}


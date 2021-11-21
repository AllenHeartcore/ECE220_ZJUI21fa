CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g
HEADERS := lab12.h Makefile
EXE := lab12
SOURCES := lab11.o lab12.o lab12main.o

.ALL: ${EXE}

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} -o $@ $<

clean::
	rm -f ${SOURCES} ${EXE}.exe.stackdump

clear: clean
	rm -f ${EXE}


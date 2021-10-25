CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -lpng -g
HEADERS := mp5.h
EXE := mp5
SOURCES := mp5.o main.o

.ALL: ${EXE}

%.o: %.c ${HEADERS} Makefile
	${CC} -c ${CFLAGS} -o $@ $<

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

clean::
	rm -f ${SOURCES} mp5 image.png


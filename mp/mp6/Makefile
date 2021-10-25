CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -lm -lpng -g
HEADERS := mp6.h
EXE := mp6
SOURCES := mp6.o main.o

.ALL: ${EXE}

%.o: %.c ${HEADERS} Makefile
	${CC} -c ${CFLAGS} -o $@ $<

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

clean::
	rm -f ${SOURCES} mp6 image.png


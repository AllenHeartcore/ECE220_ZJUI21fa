CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g -lm
HEADERS := mp5.h mp9.h Makefile
EXE := mp9
SOURCES := mp9.o mp9match.o mp9main.o

.ALL: ${EXE}

%.o: %.c ${HEADERS} Makefile
	${CC} -c ${CFLAGS} -o $@ $<

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

result.c: mp9 Makefile graph requests
	./mp9 graph requests

view_result: mp9 mp5.o mp5main.o result.o ${HEADERS}
	${CC} ${CFLAGS} -o $@ mp5.o mp5main.o result.o -lpng

image.png: view_result
	./view_result

image: image.png

clean::
	rm -f ${SOURCES} mp5.o mp5main.o result.o result.c

clear: clean
	rm -f ${EXE} image.png view_result

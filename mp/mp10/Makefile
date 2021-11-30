CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g -lm
HEADERS := mp5.h mp10.h Makefile
EXE := mp10
SOURCES := mp9.o mp10graph.o mp10alloc.o mp10match.o mp10.o mp10main.o 

.ALL: ${EXE}

%.o: %.c ${HEADERS} Makefile
	${CC} -c ${CFLAGS} -o $@ $<

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

result.c: mp10 Makefile graph requests
	./mp10 graph requests 2

genreq: mp10graph.o genreq.o ${HEADERS}
	${CC} ${CFLAGS} -o $@ mp10graph.o genreq.o

view_result: mp10 mp5.o mp5main.o result.o ${HEADERS}
	${CC} ${CFLAGS} -o $@ mp5.o mp5main.o result.o -lpng

image.png: view_result
	./view_result

image: image.png

clean::
	rm -f ${SOURCES} mp5.o mp5main.o genreq.o result.o result.c

clear: clean
	rm -f ${EXE} image.png view_result genreq

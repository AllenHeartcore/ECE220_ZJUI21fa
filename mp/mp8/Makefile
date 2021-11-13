CC=gcc
CFLAGS=-g -Wall

HEADERS=mp8.h Makefile imageData.h lodepng.h
OBJECTS=main.o lodepng.o imageData.o mp8.o mp8recurse.o

all: mp8

mp8: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o mp8

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} $< -o $@

clean::
	rm -f *.o *~

clear: clean
	rm -f mp8

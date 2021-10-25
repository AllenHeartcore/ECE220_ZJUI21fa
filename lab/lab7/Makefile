CC=gcc
CFLAGS=-g -Wall

HEADERS=lab7.h Makefile imageData.h lodepng.h
OBJECTS=main.o lodepng.o imageData.o lab7b.o

all: lab7a lab7b

lab7a: lab7a.c
	${CC} ${CFLAGS} $< -o lab7a

lab7b: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o lab7b -lm

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} $< -o $@

clean::
	rm -f *.o *~ *.stackdump

clear: clean
	rm -f lab7a lab7b

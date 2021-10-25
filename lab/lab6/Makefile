.ALL: cave

CC := gcc
CFLAGS := -g -Wall
LFLAGS := -g
HEADERS := lab6.h
OBJECTS := lab6.o main.o
LIBS := -lcurses


%.o: %.c S{HEADERS}
	${CC} ${CFLAGS} -c -o $@ $<

cave: ${OBJECTS}
	${CC} ${LFLAGS} ${OBJECTS} -o cave ${LIBS}

clean::
	rm -rf *.o *.stackdump

clear: clean
	rm cave

CC := gcc
CFLAGS :=-Wall -g
LDFLAGS := -g
HEADERS := cmd_trie.h double_list.h Makefile
EXE := lab14
SOURCES := lab14.o lab14main.o cmd_trie.o double_list.o

.ALL: ${EXE}

${EXE}: ${SOURCES}
	$(CC) ${SOURCES} -o ${EXE} ${LDFLAGS}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} -o $@ $<

clean::
	rm -f ${SOURCES} ${EXE}.exe.stackdump

clear: clean
	rm -f ${EXE}


CC = gcc
CCFLAGS = -Wall -g -pedantic
LDFLAGS = 
DEPS = myrpc.h
SOURCES = rclient1.c rclient2.c rserver.c
OBJS = ${SOURCES:.c=.o}
EXEC = rclient1 rclient2 rserver

all: ${SOURCES} ${EXEC}

%.o: %.c $(DEPS)
	$(CC) -c -g -o $@ $<

rclient1: rclient1.o
	${CC} ${CCFLAGS} -lm -o rclient1 rclient1.o
rclient2: rclient2.o
	${CC} ${CCFLAGS} -lm -o rclient2 rclient2.o
rserver: rserver.o
	${CC} ${CCFLAGS} -lm -o rserver rserver.o

# ${EXEC}: ${OBJS}
# 	${CC} ${LDFLAGS} ${OBJS} -o $@



clean:
	rm -f ${EXEC} ${OBJS}

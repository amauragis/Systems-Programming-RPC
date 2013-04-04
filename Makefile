CC = gcc
EXEC = rclient1 rclient2 rserver
CCFLAGS = -Wall -g -pedantic
OBJS = rclient1.o rclient2.o rserver.o

all: ${EXEC}

#${EXEC}: ${OBJS}
#	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

rclient1: rclient1.o
	${CC} ${CCFLAGS} -lm -o rclient1 rclient1.o
rclient2: rclient2.o
	${CC} ${CCFLAGS} -lm -o rclient2 rclient2.o
rserver: rserver.o
	${CC} ${CCFLAGS} -lm -o rserver rserver.o
    
clean:
	rm -f ${EXEC} ${OBJS}

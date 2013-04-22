CC = gcc
CCFLAGS = -Wall -g -pedantic
LDFLAGS = 
DEPS = rpcdefs.h rclient.h rserver.h
SOURCES = user_loc2rem.c user_rem2loc.c rclient.c rserver.c
OBJS = ${SOURCES:.c=.o}
EXEC = rclient1 rclient2 rserver

all: ${SOURCES} ${EXEC}

%.o: %.c $(DEPS)
	$(CC) -c -g -o $@ $<

rclient1: rclient.o user_loc2rem.o
	${CC} ${CCFLAGS} -lm -o rclient1 rclient.o user_loc2rem.o
rclient2: rclient.o user_rem2loc.o
	${CC} ${CCFLAGS} -lm -o rclient2 rclient.o user_rem2loc.o
rserver: rserver.o
	${CC} ${CCFLAGS} -lm -o rserver rserver.o

# ${EXEC}: ${OBJS}
# 	${CC} ${LDFLAGS} ${OBJS} -o $@


clean:
	rm -f ${EXEC} ${OBJS}

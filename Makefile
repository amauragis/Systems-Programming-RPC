CC = gcc
CCFLAGS = -Wall -g -std=gnu99
LDFLAGS = -lm
DEPS = rpcdefs.h rclient.h rserver.h
SOURCES = user_loc2rem.c user_rem2loc.c rclient.c rserver.c
OBJS = $(SOURCES:.c=.o)
EXEC = rclient1 rclient2 rserver

all: $(SOURCES) $(EXEC)

%.o: %.c $(DEPS)
	$(CC) -c $(CCFLAGS) -o $@ $<

rclient1: rclient.o user_loc2rem.o
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $^
rclient2: rclient.o user_rem2loc.o
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $^
rserver: rserver.o
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $^

# ${EXEC}: ${OBJS}
# 	${CC} ${LDFLAGS} ${OBJS} -o $@


clean:
	rm -f $(EXEC) $(OBJS)

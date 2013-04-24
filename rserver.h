// rserver.h
// Andrew Mauragis
// Due 4/25/13
//
// Provides function prototypes for rserver.c

#ifndef RSERVER_H
#define RSERVER_H

int call_open(int connection);
int call_close(int connection);
int call_read(int connection);
int call_write(int connection);
int call_seek(int connection);


#endif
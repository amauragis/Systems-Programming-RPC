#ifndef RSERVER_H
#define RSERVER_H

int call_open(int connection);
int call_close(int connection);
int call_read(int connection);
int call_write(int connection);
int call_seek(int connection);


#endif
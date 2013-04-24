// rpcdefs.h
// Andrew Mauragis
// Due 4/25/13
//
// Provides opcode defines, error definitions, and struct typedefs
// to allow for cleaner code.

#ifndef RPCDEFS_H
#define RPCDEFS_H

// Opcodes for RPCS
#define OPCODE_OPEN   1
#define OPCODE_CLOSE  2
#define OPCODE_READ   3
#define OPCODE_WRITE  4
#define OPCODE_SEEK   5

// Error definitions

#define SOCKET_ERROR    -2
#define BIND_ERROR      -3
#define LISTEN_ERROR    -4
#define ACCEPT_ERROR    -5
#define FORKING_ERROR   -6
#define READ_ERROR      -7
#define CONNECTION_ERROR -8
#define MALLOC_ERROR    -9
#define WRITE_ERROR     -10
#define ARGS_ERROR      -11
#define GETHOST_ERROR   -12
#define OPEN_ERROR      -13
#define STAT_ERROR      -14
#define CLOSE_ERROR     -15

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

#endif

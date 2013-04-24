// rclient.c
// Andrew Mauragis
// Due 4/25/13
//
// This file runs the remote procedure call client.  It must be compiled and linked into
// a client program with the function entry(argc int, char** argv).  The client will set
// up a socket and provide RPC calls, then call entry after appropriately trimming the
// arguments.
//
// This client works with rserver.c and implements open, close, read, write, and lseek
// on the server.

#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>

#include "rpcdefs.h"
#include "rclient.h"

// need a static file descriptor representing the connection to the server
static int connection = -1;

// main
// Connects to the server and passes the correct arguments to the client program
// after the client returns, closes the connection and exits.
//
// expects a minimum of two arguments, host name and port.  Any further arugments
// are transparantly passed to the client program
//
// returns an error code, as expected.
int main (int argc, char* argv[])
{
    // make sure we have at least 2 arguments
    if (argc < 3)
    {
        fprintf(stderr,"Not enough arguments");
        exit(ARGS_ERROR);
    }

    char* remhost;
    unsigned short remport;

    // get remote host and port
    remhost = argv[1];
    remport = atoi(argv[2]);

    // set up TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Socket error");
        exit(SOCKET_ERROR);
    }

    // initialize socket struct
    sockaddr_in_t s;
    memset(&s, 0, sizeof(sockaddr_in_t));

    // get port number and address
    struct hostent* hostaddr = gethostbyname(remhost);
    if(hostaddr == NULL)
    {
        perror("GetHost error");
        return GETHOST_ERROR;
    } 

    // populate socket struct
    s.sin_family = AF_INET;
    memcpy(&s.sin_addr, hostaddr->h_addr, hostaddr->h_length);
    s.sin_port = htons(remport);

    // attempt to connect to server
    if (-1 == connect(sock, (sockaddr_t*)&s, sizeof(sockaddr_in_t)))
    {
        perror("Cannot connect");
        return CONNECTION_ERROR;
    }

    // now that we have the socket working, we copy it into the static variable
    connection = sock;

    // everything shoudl be setup and we're ready to invoke the client program
    char** newArgv = (char**)calloc((1+(argc-3)), sizeof(char*));
    newArgv[0] = "user_program";
    int i;
    for (i=3; i<argc; i++)
    {
        newArgv[i-2] = argv[i];
    }
    int newArgc = (1+(argc-3));

    // Call user program
    int ret = entry(newArgc, newArgv);

    // Free memory
    free(newArgv);

    // close socket, we're done
    close(connection);
    return ret;
}

// Open
// RPC call, implements open() with the target on the remote server
//
// Functions by gathering arguments, packetizing them, sending them
// to the server and reading back the response for returning
//
// expects a path name, open flags, and open mode
// returns an error code/fd that matches the open() called remotely
int Open(const char* pathname, int flags, mode_t mode)
{
// puts("OPEN!");
    // first we calculate the length we need
    // 1 byte for opcode, path name, null, flags
    int path_length = strlen(pathname);
    int pkt_length = 1 + path_length + 1 + sizeof(int) + sizeof(mode_t);

    // now we initalize an rpc packet
    int pktIndex = 0;
    unsigned char pkt[pkt_length];
    memset(pkt, 0, pkt_length);

    // copy in the opcode first
    pkt[pktIndex] = OPCODE_OPEN;
    pktIndex++;

    // copy in path (first arg)
    memcpy(pkt+pktIndex, pathname, path_length);
    pktIndex += path_length;

    // copy in null terminator
    pkt[pktIndex] = 0;
    pktIndex++;

    // copy flags
    memcpy(pkt+pktIndex, &flags, sizeof(int));
    pktIndex += sizeof(int);

    // copy mode
    memcpy(pkt+pktIndex, &mode, sizeof(mode_t));
    pktIndex += sizeof(mode_t);

    if (connection == -1)
    {
        perror("Static socket invalid");
        return CONNECTION_ERROR;
    }

    // write packet to server
    int writeval = write(connection, pkt, pkt_length);
    if (writeval < 0) return WRITE_ERROR;

    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    if (readval < 0) return READ_ERROR;

    int func_return = response[0];
    int func_errno = response[1];

    // set up errno and return the return value
    errno = func_errno;
    return func_return;
}

// Close
// RPC call, implements close() with the target on the remote server
//
// Functions by gathering arguments, packetizing them, sending them
// to the server and reading back the response for returning
//
// expects a file descriptor
// returns an error code that matches the close() called remotely
int Close(int fd)
{
// puts("CLOSE!");
    // length is opcode + file descriptor
    int pktLength = (1 + sizeof(int));

    // create packet
    int index = 0;
    unsigned char pkt[pktLength];

    // copy in opcode
    pkt[index] = OPCODE_CLOSE;
    index++;

    // copy in file descriptor
    memcpy(&pkt[index], &fd, sizeof(int));

    // verify socket is correct
    if (connection == -1)
    {
        perror("Static socket invalid");
        return CONNECTION_ERROR;
    }

    // write packet to server
    int writeval = write(connection, pkt, pktLength);
    if (writeval < 0) return WRITE_ERROR;

    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    if (readval < 0) return READ_ERROR;

    int func_return = response[0];
    int func_errno = response[1];

    // set up errno and return the return value
    errno = func_errno;
    return func_return;
}

// Read
// RPC call, implements read() with the target on the remote server
//
// Functions by gathering arguments, packetizing them, sending them
// to the server and reading back the response for returning and the
// buffer that was read into.
// 
// expects a file descriptor, pointer to a buffer, and size
// return value that matches the read() called remotely
ssize_t Read(int fd, void* buf, size_t count)
{
    // length will be opcode + fd + count
    int pktLength = 1 + sizeof(int) + sizeof(size_t);

    // create a packet
    int pktIndex = 0;
    unsigned char pkt[pktLength];

    // copy in opcode
    pkt[pktIndex] = OPCODE_READ;
    pktIndex++;

    // copy fd
    memcpy(pkt+pktIndex, &fd, sizeof(int));
    pktIndex += sizeof(int);

    // copy count
    memcpy(pkt+pktIndex, &count, sizeof(size_t));
    pktIndex += sizeof(size_t);

    // verify socket is correct
    if (connection == -1)
    {
        perror("Static socket invalid");
        return CONNECTION_ERROR;
    }

    // write packet to server
    int writeval = write(connection, pkt, pktLength);
    if (writeval < 0) return WRITE_ERROR;

    // get response from server
    int response[2];
    void* readbuf[count];
    int readval = read(connection, response, 2*sizeof(int));
    if (readval < 0) return READ_ERROR;

    // get read buffer from server
    readval = read(connection, readbuf, count);

    int func_return = response[0];
    int func_errno = response[1];

    // copy read buffer into pointer specified by caller
    memcpy(buf, readbuf, count);

    // set up errno and return the return value
    errno = func_errno;
    return func_return;

}

// Write
// RPC call, implements write() with the target on the remote server
//
// Functions by gathering arguments and buffer data, packetizing them,
// sending them to the server and reading back the response
// 
// Note: we had to switch count and buffer in the packet order so we
// know how big to make the buffer, and know where count will be in
// the packet
//
// expects a file descriptor, pointer to a buffer, and size
// return value that matches the write() called remotely
ssize_t Write(int fd, const void* buf, size_t count)
{
// puts("WRITE!");
    // message length: opcode + fd + buffer length + count
    int pktLength = 1 + sizeof(int) + count + sizeof(size_t);

    // build the packet
    int pktIndex = 0;
    unsigned char pkt[pktLength];

    // copy in opcode
    pkt[pktIndex] = OPCODE_WRITE;
    pktIndex++;

    // copy in fd
    memcpy(pkt+pktIndex, &fd, sizeof(int));
    pktIndex += sizeof(int);

    // copy in count
    // had to switch this around with buf so we know where count
    // will be in the packet
    memcpy(pkt+pktIndex, &count, sizeof(size_t));
    pktIndex += sizeof(size_t);

    // copy in buf
    memcpy(pkt+pktIndex, buf, count);
    pktIndex += count;

    // verify socket is correct
    if (connection == -1)
    {
        perror("Static socket invalid");
        return CONNECTION_ERROR;
    }

    // write packet to server
    int writeval = write(connection, pkt, pktLength);
    if (writeval < 0) return WRITE_ERROR;

    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    if (readval < 0) return READ_ERROR;

    int func_return = response[0];
    int func_errno = response[1];

    // set up errno and return the return value
    errno = func_errno;
    return func_return;

}

// Lseek
// RPC call, implements lseek() with the target on the remote server
//
// Functions by gathering arguments, packetizing them, sending them
// to the server and reading back the response for returning
//
// expects a file descriptor, offset, and whence value
// returns an error code that matches the lseek() called remotely
off_t Lseek(int fd, off_t offset, int whence)
{
    // length: opcode, fd, offset, whence
    int pktLength = 1+ 2*sizeof(int) + sizeof(off_t);

    // build packet
    int pktIndex = 0;
    unsigned char pkt[pktIndex];

    // copy opcode
    pkt[pktIndex] = OPCODE_SEEK;
    pktIndex++;

    // copy fd
    memcpy(pkt+pktIndex, &fd, sizeof(int));
    pktIndex += sizeof(int);

    // copy offset
    memcpy(pkt+pktIndex, &offset, sizeof(off_t));
    pktIndex += sizeof(off_t);

    // copy whence

    memcpy(pkt+pktIndex, &whence, sizeof(int));
    pktIndex += sizeof(int);

    // verify socket is correct
    if (connection == -1)
    {
        perror("Static socket invalid");
        return CONNECTION_ERROR;
    }

    // write packet to server
    int writeval = write(connection, pkt, pktLength);
    if (writeval < 0) return WRITE_ERROR;


    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    if (readval < 0) return READ_ERROR;

    int func_return = response[0];
    int func_errno = response[1];

    // set up errno and return the return value
    errno = func_errno;
    return func_return;
}

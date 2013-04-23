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

static int connection = -1;

int main (int argc, char* argv[])
{
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

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Socket error");
        exit(SOCKET_ERROR);
    }

    // populate socket struct
    sockaddr_in_t s;
    struct hostent* hostaddr = gethostbyname(remhost);
    if(hostaddr == NULL) return GETHOST_ERROR;

    memset(&s, 0, sizeof(sockaddr_in_t));

    s.sin_family = AF_INET;
    memcpy(&s.sin_addr, hostaddr->h_addr, hostaddr->h_length);
    s.sin_port = htons(remport);

    if (-1 == connect(sock, (sockaddr_t*)&s, sizeof(sockaddr_in_t)))
    {
        // err(1, NULL);
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
    return ret;
}

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
    // do something to check writeval

    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    // do something with readval

    int func_return = response[0];
    int func_errno = response[1];
    errno = func_errno;
    return func_return;
}

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

    // check value of write here

    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    // do something with readval

    int func_return = response[0];
    int func_errno = response[1];
    errno = func_errno;
    return func_return;
}

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

    int writeval = write(connection, pkt, pktLength);
    // check writeval

    // get response from server
    int response[2];
    void* readbuf[count];
    int readval = read(connection, response, 2*sizeof(int));
    // do something with readval

    // get read buffer from server
    readval = read(connection, readbuf, count);

    int func_return = response[0];
    int func_errno = response[1];
    memcpy(buf, readbuf, count);
    errno = func_errno;
    return func_return;

}

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
    // check write val

    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    // do something with readval

    int func_return = response[0];
    int func_errno = response[1];
    errno = func_errno;
    return func_return;

}

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
    // check write val


    // get response from server
    int response[2];
    int readval = read(connection, response, 2*sizeof(int));
    // do something with readval

    int func_return = response[0];
    int func_errno = response[1];
    errno = func_errno;
    return func_return;

}

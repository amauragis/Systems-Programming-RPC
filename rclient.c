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
    char* remhost;
    unsigned short remport;

    // get remote host and port
    remhost = argv[1];
    remport = htons((unsigned short)atoi(argv[2]));

    return 0;
}

int Open(const char* pathname, int flags)
{
    // first we calculate the length we need
    // 1 byte for opcode, path name, null, flags
    int path_length = strlen(pathname);
    int pkt_length = 1 + path_length + 1 + sizeof(int);

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


    if (connection == -1)
    {
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
    return 0;
}

ssize_t Read(int fd, void* buf, size_t count)
{
    return 0;
}

ssize_t Write(int fd, const void* buf, size_t count)
{
    return 0;
}

off_t Lseek(int fd, off_t offset, int whence)
{
    return 0;
}

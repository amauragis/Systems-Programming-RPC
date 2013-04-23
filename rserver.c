#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "rserver.h"
#include "rpcdefs.h"

int main()
{
    int listener;
    int connection;
    int length;

    // create empty socket structs
    sockaddr_in_t lisSocket;

    memset(&lisSocket, 0, sizeof(sockaddr_in_t));


    // open a tcp socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1)
    {
        // socket error
        return SOCKET_ERROR;
    }

    // set up our socket struct to bind to
    lisSocket.sin_family = (short) AF_INET;
    lisSocket.sin_addr.s_addr = htonl(INADDR_ANY); // Accept any internet address
    lisSocket.sin_port = htons(0); // Bind to any open port

    // now we bind
    int bindval;
    length = sizeof(lisSocket);
    bindval = bind(listener, (sockaddr_t*) &lisSocket, length);
    if (bindval == -1)
    {
        // bind error
        return BIND_ERROR;
    }

    // print out the port number of the socket
    getsockname(listener, (sockaddr_t*) &lisSocket, (socklen_t*)&length);
    printf("Server bound to port %d\n", ntohs(lisSocket.sin_port));

    // begin listening, we only accept one connection in the listening queue
    if (-1 == listen(listener,1))
    {
        return LISTEN_ERROR;
    }

    // main server loop after socket is setup and we are listening
    while (1) 
    {
        sockaddr_in_t conSocket;
        int conSocket_len = sizeof(sockaddr_in_t);
        memset(&conSocket, 0, conSocket_len);

        // now we accept a new connection
        connection = accept(listener, (sockaddr_t*) &conSocket, (socklen_t*)&conSocket_len);
        if (connection == -1)
        {
            return ACCEPT_ERROR;
        }
        char host[NI_MAXHOST];
        getnameinfo((struct sockaddr*)&conSocket, conSocket_len,host,sizeof(host), NULL, 0, 0);
        

        printf("Connection accepted from: %s\n", host);

        // now we have to fork to make sure we can allow additional connections
        pid_t pid = fork();
        if (pid == -1)
        {
            return FORKING_ERROR;
        }
        else if (pid == 0)
        {
            // Child.  Handle this connection by reading the opcode
            close(listener);
            while (1)
            {
                
                unsigned char opcode;
                int readval = read(connection, &opcode, 1);
                if (readval < 0)
                {
                    return READ_ERROR;
                }
                else if (readval == 0)
                {
                    // nothing read
                    exit(0);
                }

                int ret;
                // opcode selects correct function to call
                switch (opcode)
                {
                    case OPCODE_OPEN:
                        puts("Open!");
                        ret = call_open(connection);
                        break;
                    case OPCODE_CLOSE:
                        puts("Close!");
                        ret = call_close(connection);
                        break;
                    case OPCODE_READ:
                        puts("Read!");
                        ret = call_read(connection);
                        break;
                    case OPCODE_WRITE:
                        puts("Write!");
                        ret = call_write(connection);
                        break;
                    case OPCODE_SEEK:
                        puts("Seek!");
                        ret = call_seek(connection);
                        break;
                }
                switch(ret)
                {
                    case MALLOC_ERROR:
                        perror("Malloc error");
                        break;

                }
                fprintf(stderr, "Exit code: %d\n",ret);
                fflush(stderr);
                // exit(ret);
                // break; something here is awry
            }
        }
        else
        {
            // Parent, drop this connection, child has it
            close(connection);
        }

    }

}

int call_open(int connection)
{
    int bufSize = 80;
    char* pathBuf = malloc(bufSize*sizeof(char));
    if (pathBuf == NULL) return MALLOC_ERROR;

    int index = 0;
    char currChar = 0;
    int readval;
    do 
    {
        readval = read(connection, &currChar, 1);
        if (readval == -1) return READ_ERROR;
        if (readval == 0)
        {
            // socket closed?
            perror("nothing to read. Socket closed?");
        }

        // write current character into path buffer
        pathBuf[index] = currChar;
        index++;

        // resize the buffer if we run out of space
        if(index == bufSize)
        {
            bufSize *= 2;
            pathBuf = realloc(pathBuf, bufSize);
            if (pathBuf == NULL) return MALLOC_ERROR;
        }

    } while (currChar != 0);

    // now we have to get the flags
    int flags = 0;
    readval = read(connection, &flags, sizeof(int));
    if (readval == 0)
    {
        // socket closed?
        perror("nothing to read. Socket closed?");
    }
    else if (readval != sizeof(int)) return READ_ERROR;

    // now we need to get the mode
    mode_t mode = 0;
    readval = read(connection, &mode, sizeof(mode_t));
    if (readval == 0)
    {
        // socket closed?
        perror("nothing to read. Socket closed?");
    }
    else if (readval != sizeof(int)) return READ_ERROR;

    // we have built our command... lets try it
    int func_ret = open(pathBuf, flags, mode);
    int func_errno = errno;
    printf("opened fd: %d\n", func_ret);

    // now we have to write our message back, which is the return and error values
    int pktLength = 2*sizeof(int);
    int pkt[2];
    pkt[0] = func_ret;
    pkt[1] = func_errno;

    int writeval = write(connection, pkt, pktLength);
    if (writeval == 0)
    {
        // socket closed?
        perror("cannot write. Socket closed?");
    }
    else if (writeval < pktLength) return WRITE_ERROR;

    // everything went as planned
    return 0;
}

int call_close(int connection)
{
    // read the file descriptor from the packet
    int fd = 0;
    int readval = read(connection, &fd, sizeof(int));
    if (readval == -1) return READ_ERROR;
    if (readval == 0)
    {
        // socket closed?
        perror("nothing to read. Socket closed?");
    }
    // have all parameters now run command
    int func_ret = close(fd);
    int func_errno = errno;

    // now we have to write our message back, which is the return and error values
    int pktLength = 2*sizeof(int);
    int pkt[2];
    pkt[0] = func_ret;
    pkt[1] = func_errno;

    int writeval = write(connection, pkt, pktLength);
    if (writeval == 0)
    {
        // socket closed?
        perror("cannot write. Socket closed?");
    }
    else if (writeval < pktLength) return WRITE_ERROR;

    return 0;

}

int call_read(int connection)
{
    return 0;
}

int call_write(int connection)
{
    // Read in fd
    int fd = 0;
    int readval = read(connection, &fd, sizeof(int));
    if (readval == -1) return READ_ERROR;
    if (readval == 0)
    {
        // socket closed?
        perror("nothing to read. Socket closed?");
    }

    // read in count
    size_t count = 0;
    readval = read(connection, &count, sizeof(size_t));
    if (readval == -1) return READ_ERROR;
    if (readval == 0)
    {
        // socket closed?
        perror("nothing to read. Socket closed?");
    }

    // read in the data
    unsigned char buf[count];
    readval = read(connection, buf, count);
    if (readval == -1) return READ_ERROR;
    if (readval == 0)
    {
        // socket closed?
        perror("nothing to read. Socket closed?");
    }

    // all data in, now run command
    printf("FD: %d\n", fd);
    int func_ret = write(fd, buf, count);
    int func_errno = errno;
    perror("error?");

    // now we have to write our message back, which is the return and error values
    int pktLength = 2*sizeof(int);
    int pkt[2];
    pkt[0] = func_ret;
    pkt[1] = func_errno;

    int writeval = write(connection, pkt, pktLength);
    if (writeval == 0)
    {
        // socket closed?
        perror("cannot write. Socket closed?");
    }
    else if (writeval < pktLength) return WRITE_ERROR;

    return 0;

}

int call_seek(int connection)
{
    return 0;
}
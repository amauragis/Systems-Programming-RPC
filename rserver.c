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
    getsockname(listener, (sockaddr_t*) &lisSocket, &length);
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
        connection = accept(listener, (sockaddr_t*) &conSocket, &conSocket_len);
        if (connection == -1)
        {
            return ACCEPT_ERROR;
        }

        // now we have to fork to make sure we can allow additional connections
        pid_t pid = fork();
        if (pid == -1)
        {
            return FORKING_ERROR;
        }
        else if (pid == 0)
        {
            // Child.  Handle this connection by reading the opcode
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
                }

                // opcode selects correct function to call
                switch (opcode)
                {
                    case OPCODE_OPEN:
                        call_open(connection);
                        break;
                    case OPCODE_CLOSE:
                        call_close(connection);
                        break;
                    case OPCODE_READ:
                        call_read(connection);
                        break;
                    case OPCODE_WRITE:
                        call_write(connection);
                        break;
                    case OPCODE_SEEK:
                        call_seek(connection);
                        break;
                }
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

}

int call_close(int connection)
{
    
}

int call_read(int connection)
{
    
}

int call_write(int connection)
{
    
}

int call_seek(int connection)
{
    
}
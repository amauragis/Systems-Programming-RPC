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

#include "myrpc.h"

int main()
{
    int listener;
    int connection;
    int length;

    sockaddr_in_t lisSocket;
    sockaddr_in_t conSocket;

    // open a tcp socket
    listener = socket(AF_INET, SOCK_STREAM, 0);

    // set up our socket struct to bind to
    lisSocket.sin_family = (short) AF_INET;
    lisSocket.sin_addr.s_addr = htonl(INADDR_ANY); // Accept any internet address
    lisSocket.sin_port = htons(0); // Bind to any open port

    // now we bind
    // todo: error checking
    int bindval;
    length = sizeof(lisSocket);
    bindval = bind(listener, (sockaddr_t*) &lisSocket, length);

    // print out the port number of the socket
    getsockname(listener, (sockaddr_t*) &lisSocket, &length);
    printf("Server bound to port %d\n", ntohs(lisSocket.sin_port));

    // begin listening, we only accept one connection in the listening queue
    listen(listener,1);
    length = sizeof(conSocket);

    // now we accept a new connection
    connection = accept(listener, (sockaddr_t*) &conSocket, &length);

    while (1) 
    {
        // set up select
        fd_set readfds, writefds;
        struct timeval tv;
        tv.tv_sec=0;
        tv.tv_usec=0;
        char ch;
        FD_ZERO(&readfds);
        // FD_ZERO(&writefds);
        FD_SET(connection, &readfds);
        // FD_SET(conn, &writefds);

        int selectval = select(0, &readfds, NULL, NULL, &tv);
        // puts("through select");
        if (selectval == -1)
            perror("Something bad happened with select()");
        else
        {
            if (FD_ISSET(connection, &readfds))
            {
               
                while(read(connection, &ch, 1) != 0)
                    putchar(ch);   
            }
        }
    }

}

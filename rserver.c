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

    sockaddr_in_t ourSocket;

    // open a tcp socket
    listener = socket(AF_INET, SOCK_STREAM, 0);

}

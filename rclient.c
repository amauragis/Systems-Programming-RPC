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

#include "rpcdefs.h"

int main (int argc, char* argv[])
{
    char* remhost;
    unsigned short remport;

    // get remote host and port
    remhost = argv[1];
    remport = htons((unsigned short)atoi(argv[2]));

    return 0;
}

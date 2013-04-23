#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"

int entry(int argc, char* argv[])
{
    int rslt = Open("myfile1", O_CREAT | O_APPEND, 0644);
    printf("I opened fd: %d\n", rslt);

    printf("I'm going to try to write 'dicks dicks dicks\\n' to a file\n");
    int writeval = Write(rslt,"dicks dicks dicks\n", 19);

    printf("Write return: %d\n", writeval);

    int closeval = Close(rslt);
    printf("Close result: %d\n",closeval);

    return 0;

}

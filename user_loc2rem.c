#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"

int entry(int argc, char* argv[])
{
    int rslt = Open("myfile1", O_CREAT | O_APPEND, 0744);
    puts("I opened!");
    int closeval = Close(rslt);
    printf("Open result: %d\n",closeval);

}

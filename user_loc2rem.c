#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"

int entry()
{
    int rslt = Open("myfile1", O_CREAT | O_APPEND);
    printf("Open result: %d\n",rslt);

}

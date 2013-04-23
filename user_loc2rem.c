#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"

int entry(int argc, char* argv[])
{
    int rslt = Open("myfile1", O_CREAT | O_APPEND | O_WRONLY, 0644);
    printf("I opened fd: %d\n", rslt);

    printf("I'm going to try to write 'dicks dicks dicks\\n' to a file\n");
    int writeval = Write(rslt,"dicks dicks dicks\n", 19);

    printf("Write return: %d\n", writeval);

    int closeval = Close(rslt);
    printf("Close return: %d\n",closeval);

    // now lets try to read it back and print it out
    int fd = Open("myfile1", O_RDONLY, 0000);
    printf("I opened fd %d for reading.\n", fd);
    char buf[18];
    int readval = Read(fd, &buf, 17);
    printf("I read %d bytes.\n", readval);
    buf[17] = 0;

    closeval = Close(fd);
    printf("I closed with return val %d\n", closeval);

    printf("The buffer I read was:<%s>\n", buf);

    fd = Open("myfile1", O_WRONLY, 0644);
    Lseek(fd, 0, SEEK_SET);
    Write(fd, "DICK", 4);
    Close(fd);

    return 0;

}

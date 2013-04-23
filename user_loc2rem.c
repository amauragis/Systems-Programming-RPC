#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"

int entry(int argc, char* argv[])
{
    int rslt = Open("BULLETS", O_CREAT | O_WRONLY, 0644);
    printf("I opened fd: %d\n", rslt);
    perror("Open");

    printf("I'm going to try to write 'dicks dicks dicks\\n' to a file\n");
    int writeval = Write(rslt,"dicks dicks dicks\n", 19);
    perror("Write");

    printf("Write return: %d\n", writeval);

    int closeval = Close(rslt);
    perror("Close");
    printf("Close return: %d\n",closeval);


    // now lets try to read it back and print it out
    int fd = Open("BULLETS", O_RDONLY, 0000);
    perror("Open");
    printf("I opened fd %d for reading.\n", fd);
    char buf[18];
    int readval = Read(fd, &buf, 17);
    perror("Read");
    printf("I read %d bytes.\n", readval);
    buf[17] = 0;

    closeval = Close(fd);
    perror("Close");
    printf("I closed with return val %d\n", closeval);

    printf("The buffer I read was:<%s>\n", buf);

    fd = Open("BULLETS", O_WRONLY, 0644);
    perror("Open");
    Lseek(fd, 0, SEEK_SET);
    perror("Lseek");
    Write(fd, "DICK", 4);
    perror("Write");
    Close(fd);
    perror("Close");

    return 0;

}

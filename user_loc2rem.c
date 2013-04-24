#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"
#include "rpcdefs.h"

#define BUFFER_SIZE 512

int entry(int argc, char* argv[])
{
    
    // first we pull in the file name from the argument array
    if (argc < 2)
    {
        fprintf(stderr,"Not enough arguments.  Specify a filename\n");
        return ARGS_ERROR;
    }
    char* filename = argv[1];

    // open up the file to copy remotely
    int locfd = open(filename, O_RDONLY, 0000);
    if (locfd < 0)
    {
        perror("[Local] Open Error");
        return OPEN_ERROR;
    }
    // get permissions of local file
    struct stat s;
    if (0 > fstat(locfd, &s))
    {
        perror("[Local] Stat Error");
        return STAT_ERROR;
    }

    // open/create the remote copy
    int remfd = Open(filename, O_CREAT | O_WRONLY, s.st_mode);
    if (remfd < 0)
    {
        perror("[Remote] Open Error");
        return OPEN_ERROR;
    }

    // now we read locally and write remotely
    int bytesRead;
    unsigned char buf[BUFFER_SIZE];
    while ((bytesRead = read(locfd, &buf, BUFFER_SIZE)) > 0)
    {
        if (0 >= Write(remfd, &buf, bytesRead))
        {
            perror("[Remote] Write Error");
            return WRITE_ERROR;
        }

    }
    if (bytesRead < 0)
    {
        perror("[Local] Read Error");
        return READ_ERROR;
    }

    // now we close the local and remote fds
    if (0 > close(locfd))
    {
        perror("[Local] Close Error");
        return CLOSE_ERROR;
    }
    if (0 > Close(remfd))
    {
        perror("[Remote] Close Error");
        return CLOSE_ERROR;
    }

    return 0;

}


/*
TESTING CODE:

int rslt = Open("BULLETS", O_CREAT | O_WRONLY, 0644);
// printf("I opened fd: %d\n", rslt);
// perror("Open");

// printf("I'm going to try to write 'dicks dicks dicks\\n' to a file\n");
int writeval = Write(rslt,"dicks dicks dicks\n", 19);
perror("Write");

// printf("Write return: %d\n", writeval);

int closeval = Close(rslt);
perror("Close");
// printf("Close return: %d\n",closeval);


// now lets try to read it back and print it out
int fd = Open("BULLETS", O_RDONLY, 0000);
perror("Open");
// printf("I opened fd %d for reading.\n", fd);
char buf[18];
int readval = Read(fd, &buf, 17);
perror("Read");
// printf("I read %d bytes.\n", readval);
buf[17] = 0;

closeval = Close(fd);
perror("Close");
// printf("I closed with return val %d\n", closeval);

// printf("The buffer I read was:<%s>\n", buf);

fd = Open("BULLETS", O_WRONLY, 0644);
perror("Open");
Lseek(fd, 0, SEEK_SET);
perror("Lseek");
Write(fd, "DICK", 4);
perror("Write");
Close(fd);
perror("Close");

*/

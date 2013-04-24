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

    // open up the file to write to
    int locfd = open(filename, O_CREAT| O_WRONLY, 0644);
    if (locfd < 0)
    {
        perror("[Local] Open Error");
        return OPEN_ERROR;
    }
    

    // open the remote copy
    int remfd = Open(filename, O_RDONLY, 000);
    if (remfd < 0)
    {
        perror("[Remote] Open Error");
        return OPEN_ERROR;
    }

    // now we read remotely and write locally
    int bytesRead;
    unsigned char buf[BUFFER_SIZE];
    while ((bytesRead = Read(remfd, &buf, BUFFER_SIZE)) > 0)
    {
        if (0 >= write(locfd, &buf, bytesRead))
        {
            perror("[Local] Write Error");
            return WRITE_ERROR;
        }

    }
    if (bytesRead < 0)
    {
        perror("[Remote] Read Error");
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
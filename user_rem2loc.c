// user_rem2loc.c (rclient2)
// Andrew Mauragis
// Due 4/25/13
//
// This is a sample client program for linking with rclient.
// This program copies a remote file specified by the argument array to the
// local machine in the current working directory.
//
// Note, because there are no provisions for a remote Stat command, file
// permissions are not perserved.

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "rclient.h"
#include "rpcdefs.h"

#define BUFFER_SIZE 512

// entry function
//
// To be called from rclient's main function, argc and argv are trimmed
// in rclient to remove the host address and port number
//
// returns an error code like a main() would.
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

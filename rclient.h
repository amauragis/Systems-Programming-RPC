#ifndef RCLIENT_H
#define RCLIENT_H

int Open(const char* pathname, int flags, mode_t mode);
int Close(int fd);
ssize_t Read(int fd, void* buf, size_t count);
ssize_t Write(int fd, const void* buf, size_t count);
off_t Lseek(int fd, off_t offset, int whence);

int entry(int argc, char* argv[]);

#endif
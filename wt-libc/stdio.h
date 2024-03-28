#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>
#include <sys/types.h>

#define PATH_MAX 4096
#define NAME_MAX 255

#define O_RDONLY 0x0
#define O_WRONLY 0x1
#define O_RDWR 0x2
#define O_ACCMODE 0x3
#define O_NONBLOCK 0x4
#define O_APPEND 0x8
#define O_CREAT 0x200
#define O_TRUNC 0x400
#define O_EXCL 0x800
#define FMARK 0x1000
#define FDEFER 0x2000
#define FWASLOCKED 0x4000
#define FHASLOCK FWASLOCKED
#define O_EVTONLY 0x8000
#define FWASWRITTEN 0x10000
#define O_NOCTTY 0x20000
#define FNOCACHE 0x40000
#define FNORDAHEAD 0x80000
#define O_DIRECTORY 0x100000
#define O_SYMLINK 0x200000

struct dirent64
{
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

int open(const char *filename, int flags);
int close(int fd);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int exit(int status);
int read(int fd, void *buf, size_t count);
char *path_concat(char *absolute, const char *relative);
char *path_normalize(char *path);
int chdir(const char *path);
int getdents64(int fd, void *dirp, int count);

#endif
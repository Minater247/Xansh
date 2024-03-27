#include <stdint.h>
#include <stddef.h>

#include <string.h>

/**
 * Open a file using the syscall interface
 * 
 * @param filename The name of the file to open
 * @param flags The flags to open the file with
 * 
 * @return The file descriptor
*/
int open(const char *filename, int flags) {
    int fd;
    asm volatile (
        "mov $2, %%rax;"
        "syscall;"
        : "=a" (fd)
        : "D" (filename), "S" (flags)
        : "rcx", "r11", "memory"
    );
    return fd;
}

/**
 * Close a file using the syscall interface
 * 
 * @param fd The file descriptor to close
 * 
 * @return The return value of the syscall
*/
int close(int fd) {
    int ret;
    asm volatile (
        "mov $3, %%rax;"
        "syscall;"
        : "=a" (ret)
        : "D" (fd)
        : "rcx", "r11", "memory"
    );
    return ret;
}

/**
 * Read from a file using the syscall interface
 * 
 * @param fd The file descriptor to read from
 * @param buf The buffer to read into
 * @param count The number of bytes to read
 * 
 * @return The number of bytes read
*/
int read(int fd, void *buf, size_t count) {
    int ret;
    asm volatile (
        "mov $0, %%rax;"
        "syscall;"
        : "=a" (ret)
        : "D" (fd), "S" (buf), "d" (count)
        : "rcx", "r11", "memory"
    );
    return ret;
}

/**
 * Write to a file using the syscall interface
 * 
 * @param fd The file descriptor to write to
 * @param buf The buffer to write from
 * @param count The number of bytes to write
 * 
 * @return The number of bytes written
*/
int write(int fd, const void *buf, size_t count) {
    int ret;
    asm volatile (
        "mov $1, %%rax;"
        "syscall;"
        : "=a" (ret)
        : "D" (fd), "S" (buf), "d" (count)
        : "rcx", "r11", "memory"
    );
    return ret;
}

/**
 * Exit the program using the syscall interface
 * 
 * @param status The status to exit with
 * 
 * @return The return value of the syscall
*/
int exit(int status) {
    asm volatile (
        "mov $60, %%rax;"
        "syscall;"
        :
        : "D" (status)
        : "rcx", "r11", "memory"
    );
    return 0;
}

/**
 * Change the working directory using the syscall interface
 * 
 * @param path The path to change to
 * 
 * @return The return value of the syscall
*/
int chdir(const char *path) {
    int ret;
    asm volatile (
        "mov $80, %%rax;"
        "syscall;"
        : "=a" (ret)
        : "D" (path)
        : "rcx", "r11", "memory"
    );
    return ret;
}

char *path_concat(char* absolute, const char* relative) {
    if (relative[0] == '/') {
        strcpy(absolute, relative);
        return absolute;
    }

    char* token = strtok((char *)relative, "/");
    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            // Find the last '/' in the absolute path and terminate the string there
            char* last_slash = strrchr(absolute, '/');
            if (last_slash != NULL) {
                *last_slash = '\0';
            }
        } else if (strcmp(token, ".") != 0) {
            // Append '/' and the token to the absolute path
            strcat(absolute, "/");
            strcat(absolute, token);
        }
        token = strtok(NULL, "/");
    }

    return absolute;
}

char *path_normalize(char *path) {
    // remove duplicate '/'
    char *src = path;
    char *dst = path;
    while (*src) {
        *dst = *src;
        if (*src == '/') {
            while (*src == '/') {
                src++;
            }
        } else {
            src++;
        }
        dst++;
    }

    return path;
}
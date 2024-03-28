#include <stdint.h>
#include <stdbool.h>

#include "wt-libc/stdio.h"
#include "wt-libc/string.h"
#include "wt-libc/sys/types.h"

#define MAX_LENGTH 256

char pwd[PATH_MAX] = "/";
char pre[PATH_MAX + 64];
char old_pwd[PATH_MAX];

/**
 * Read a string with a given prompt, writing progress to the console
 *
 * @param pre_string The string to write before the input
 * @param keyboard_fd The file descriptor for the keyboard
 * @param output_fd The file descriptor for the output
 * @param buffer The buffer to store the string in
 */
void read_string(char *pre_string, int keyboard_fd, int output_fd, char *buffer)
{
    int i = 0;
    write(output_fd, pre_string, strlen(pre_string));
    while (i < MAX_LENGTH - 1)
    {
        int num = read(keyboard_fd, &buffer[i], 1);
        if (num > 0)
        {
            if (buffer[i] == '\n' || buffer[i] == '\r')
            {
                break;
            }
            else if (buffer[i] == '\b' || buffer[i] == 127)
            { // ASCII for backspace
                if (i > 0)
                {
                    i--;
                    write(output_fd, "\b \b", 3);
                }
            }
            else
            {
                i++;
            }
            write(output_fd, "\r", 1);
            write(output_fd, pre_string, strlen(pre_string));
            write(output_fd, buffer, i);
        }
    }
    buffer[i] = '\0';          // Null terminate the string
    write(output_fd, "\n", 1); // Write a newline to the console
}

/**
 * Get the nth argument from a string
 *
 * @param string The string to get the argument from
 * @param buffer The buffer to store the argument in
 *
 * @return The nth argument
 */
char *get_nth_arg(char *string, char *buffer, int n)
{
    int i = 0;
    int arg_num = 0;
    while (arg_num < n)
    {
        while (string[i] != ' ' && string[i] != '\0')
        {
            i++;
        }
        if (string[i] == '\0')
        {
            buffer[0] = '\0';
            return buffer;
        }
        while (string[i] == ' ')
        {
            i++;
        }
        arg_num++;
    }
    int j = 0;
    while (string[i] != ' ' && string[i] != '\0')
    {
        buffer[j] = string[i];
        i++;
        j++;
    }
    buffer[j] = '\0';
    return buffer;
}

int get_num_args(char *string)
{
    int i = 0;
    int num_args = 0;
    while (string[i] != '\0')
    {
        if (string[i] != ' ')
        {
            num_args++;
            while (string[i] != ' ' && string[i] != '\0')
            {
                i++;
            }
        }
        else
        {
            i++;
        }
    }
    return num_args;
}

bool isDir(char *path)
{
    int fd = open(path, O_DIRECTORY);
    if (fd < 0)
    {
        return false;
    }
    close(fd);
    return true;
}

int main()
{
    int fd = open("/dev/so0/", 0);
    int keyboard_fd = open("/dev/kb0/", 0);

    // Simple MOTD
    write(fd, "Welcome to the simple XanaduOS shell - Xansh!\n", 46);
    write(fd, "Type 'exit' to exit the shell.\n", 31);

    // read a string and write it until "exit" is entered
    char buffer[MAX_LENGTH];
    while (1)
    {
        // write(fd, "[root@xanaduOS]# ", 17);
        strcpy(pre, "[root@xanaduOS] ");
        strcat(pre, pwd);
        strcat(pre, "# ");
        read_string(pre, keyboard_fd, fd, buffer);

        // Check the command
        char command[MAX_LENGTH];
        get_nth_arg(buffer, command, 0);
        if (strcmp(command, "exit") == 0)
        {
            // get the argument
            get_nth_arg(buffer, command, 1);
            if (strlen(command) > 0)
            {
                exit(atoi(command));
            }
            else
            {
                exit(0);
            }
        }
        else if (strcmp(command, "echo") == 0)
        {
            // write args until string end
            int i = 1;
            while (1)
            {
                get_nth_arg(buffer, command, i);
                if (strlen(command) == 0)
                {
                    break;
                }
                write(fd, command, strlen(command));
                write(fd, " ", 1);
                i++;
            }
            write(fd, "\n", 1);
        }
        else if (strcmp(command, "cat") == 0)
        {
            // read all the files
            int i = 1;
            while (1)
            {
                get_nth_arg(buffer, command, i);
                if (strlen(command) == 0)
                {
                    break;
                }
                int file_fd = open(command, 0);

                if (file_fd < 0)
                {
                    write(fd, "File not found\n", 15);
                }
                else
                {
                    // a hack, if it succeeds with O_DIRECTORY, then it's a directory and we don't want to open it
                    if (isDir(command))
                    {
                        write(fd, "Cannot cat a directory\n", 23);
                        close(file_fd);
                        i++;
                        continue;
                    }

                    char file_buffer[MAX_LENGTH];
                    int num = read(file_fd, file_buffer, MAX_LENGTH);
                    while (num > 0)
                    {
                        write(fd, file_buffer, num);
                        num = read(file_fd, file_buffer, MAX_LENGTH);
                    }
                    write(fd, "\n", 1);
                    close(file_fd);
                }
                i++;
            }
        }
        else if (strcmp(command, "cd") == 0)
        {
            // only takes 1 argument
            strcpy(old_pwd, pwd);
            get_nth_arg(buffer, command, 1);
            path_normalize(path_concat(pwd, command));
            if (!isDir(pwd))
            {
                strcpy(pwd, old_pwd);
                write(fd, "Directory not found\n", 20);
                continue;
            }
            if (chdir(pwd) < 0)
            {
                strcpy(pwd, old_pwd);
                write(fd, "Failed to change directory\n", 27);
            }
        }
        else if (strcmp(command, "ls") == 0)
        {
            int i = 1;
            while (1)
            {
                get_nth_arg(buffer, command, i);
                if (strlen(command) == 0)
                {
                    break;
                }
                int dir_fd = open(command, 0);
                if (dir_fd < 0)
                {
                    write(fd, "Failed to open directory\n", 25);
                }
                else
                {
                    if (get_num_args(buffer) > 1)
                    {
                        write(fd, command, strlen(command));
                        write(fd, ":\n", 2);
                    }
                    char buf[1024];
                    int nread;
                    struct dirent64 *d;
                    int bpos;

                    for (;;)
                    {
                        nread = getdents64(dir_fd, buf, 1024);
                        if (nread == -1)
                            break;

                        if (nread == 0)
                            break;

                        int did = 0;

                        for (bpos = 0; bpos < nread;)
                        {
                            if (did == 8) {
                                break;
                            }
                            d = (struct dirent64 *)(buf + bpos);
                            write(fd, d->d_name, strlen(d->d_name));
                            write(fd, "\n", 1);
                            bpos += d->d_reclen;
                            did++;
                        }
                    }
                    close(dir_fd);
                }
                i++;
            }

            // if there were no arguments, list the current directory
            if (get_num_args(buffer) == 1)
            {
                int dir_fd = open(pwd, 0);
                if (dir_fd < 0)
                {
                    write(fd, "Failed to open directory\n", 25);
                }
                else
                {
                    write(fd, pwd, strlen(pwd));
                    write(fd, ":\n", 2);

                    char buf[1024];
                    int nread;
                    struct dirent64 *d;
                    int bpos;

                    for (;;)
                    {
                        nread = getdents64(dir_fd, buf, 1024);
                        if (nread == -1)
                            break;

                        if (nread == 0)
                            break;

                        for (bpos = 0; bpos < nread;)
                        {
                            d = (struct dirent64 *)(buf + bpos);
                            write(fd, d->d_name, strlen(d->d_name));
                            write(fd, "\n", 1);
                            bpos += d->d_reclen;
                        }
                    }
                    close(dir_fd);
                }
            }
        }
        else if (strcmp(command, "pwd") == 0)
        {
            write(fd, pwd, strlen(pwd));
            write(fd, "\n", 1);
        }
        else if (strcmp(command, "clear") == 0) {
            write(fd, "\033[2J\033[1;1H", 10);
        }
        else
        {
            write(fd, "Command not found\n", 18);
        }
    }

    close(fd);

    exit(0);
}
#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define MAX_LENGTH 256

char pwd[PATH_MAX] = "/";
char pre[PATH_MAX + 64];
char old_pwd[PATH_MAX];

uint64_t zero = 0;

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
    bool in_quotes = false;
    while (arg_num < n)
    {
        while ((string[i] != ' ' || in_quotes) && string[i] != '\0')
        {
            if (string[i] == '\"' && (i == 0 || string[i-1] != '\\'))
            {
                in_quotes = !in_quotes;
            }
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
    while ((string[i] != ' ' || in_quotes) && string[i] != '\0')
    {
        if (string[i] == '\"' && (i == 0 || string[i-1] != '\\'))
        {
            in_quotes = !in_quotes;
        }
        else
        {
            buffer[j] = string[i];
            j++;
        }
        i++;
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

bool fileExists(char *path)
{
    int fd = open(path, 0);
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
        strcpy(pre, "\033[0m[\033[38;5;160mroot\033[0m@\033[38;5;27mxanaduOS\033[0m] \033[38;5;41m");
        strcat(pre, pwd);
        strcat(pre, "\033[0m# ");
        read_string(pre, keyboard_fd, fd, buffer);

        // Ensure there is an even number of unescaped quotes
        int num_quotes = 0;
        for (size_t i = 0; i < strlen(buffer); i++)
        {
            if (buffer[i] == '"')
            {
                num_quotes++;
            }
            if (buffer[i] == '\\' && buffer[i + 1] == '"')
            {
                num_quotes--;
                i++;
            }
        }
        if (num_quotes % 2 != 0)
        {
            write(fd, "Unmatched quotes\n", 17);
            continue;
        }

        // Check the command
        char command[MAX_LENGTH];
        get_nth_arg(buffer, command, 0);
        if (strcmp(command, "exit") == 0)
        {
            // get the argument
            get_nth_arg(buffer, command, 1);
            write(fd, "Goodbye!\n", 9);
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
            get_nth_arg(buffer, command, 1);
            path_normalize(command);
            if (!isDir(command))
            {
                write(fd, "Directory not found\n", 20);
                continue;
            }
            if (chdir(command) < 0)
            {
                write(fd, "Failed to change directory\n", 27);
            }
            getcwd(pwd, PATH_MAX);
        }
        else if (strcmp(command, "ls") == 0)
        {
            int i = 1;
            int num_args = get_num_args(buffer);
            char buf[1024];
            int nread;
            struct dirent64 *d;
            int bpos;
            char dir_path[PATH_MAX];
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
                    if (num_args > 2)
                    {
                        write(fd, command, strlen(command));
                        write(fd, ":\n", 2);
                    }

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
                            if (num_args > 2) {
                                write(fd, "\t", 1);
                            }

                            // convert to absolute and check if it's a directory
                            strcpy(dir_path, command);
                            strcat(dir_path, "/");
                            strcat(dir_path, d->d_name);
                            if (isDir(dir_path))
                            {
                                write(fd, "\033[38;5;27m", 10);
                            } else {
                                write(fd, "\033[0m", 4);
                            }

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
            if (num_args == 1)
            {
                int dir_fd = open(pwd, 0);
                if (dir_fd < 0)
                {
                    write(fd, "Failed to open directory\n", 25);
                }
                else
                {

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

                            // convert to absolute and check if it's a directory
                            strcpy(dir_path, pwd);
                            strcat(dir_path, "/");
                            strcat(dir_path, d->d_name);
                            if (isDir(dir_path))
                            {
                                write(fd, "\033[38;5;27m", 10);
                            } else {
                                write(fd, "\033[0m", 4);
                            }

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
        else if(command[0] == '.' || command[0] == '/') {
            // Make sure the file exists
            if (!fileExists(command))
            {
                write(fd, "File not found\n", 15);
                continue;
            }

            if (isDir(command))
            {
                write(fd, "Cannot execute a directory\n", 27);
                continue;
            }

            char path_command[PATH_MAX];
            strcpy(path_command, command);

            int pid = fork();
            if (pid == 0)
            {
                execv((const char *)&path_command, (char *const *)&zero, (char *const *)&zero);
                write(fd, "Command not found\n", 18);
                exit(1);
            }
            else
            {
                int status;
                int val = wait4(pid, &status, 0, NULL);
                if (val < 0) {
                    write(fd, "Error waiting for child process\n", 32);
                }
            }
        }
        else
        {
            write(fd, "Command not found\n", 18);
        }
    }

    close(fd);

    exit(0);
}
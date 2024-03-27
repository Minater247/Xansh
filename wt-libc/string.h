#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>

int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
char* strncpy(char* dest, const char* src, size_t n);
char* strchr(const char* str, int c);
size_t strlen(const char* str);
int atoi(const char* str);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
size_t strspn(const char *str, const char *delim);
size_t strcspn(const char *str, const char *delim);
char *strtok(char *str, const char *delim);
char *strrchr(const char *str, int c);

#endif
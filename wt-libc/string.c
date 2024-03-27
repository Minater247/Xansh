#include <stdint.h>
#include <stddef.h>



/**
 * Compare two strings
 * 
 * @param str1 The first string
 * @param str2 The second string
 * 
 * @return The difference between the two strings
 */
int strcmp(const char* str1, const char* str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

/**
 * Compare two strings up to a certain length
 * 
 * @param str1 The first string
 * @param str2 The second string
 * @param n The number of characters to compare
 * 
 * @return The difference between the two strings
 */
int strncmp(const char* str1, const char* str2, size_t n) {
    while (n > 0 && *str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
        n--;
    }
    return n == 0 ? 0 : *str1 - *str2;
}

/**
 * Copy a string up to a certain length
 * 
 * @param dest The destination string
 * @param src The source string
 * @param n The number of characters to copy
 * 
 * @return The destination string
 */
char* strncpy(char* dest, const char* src, size_t n) {
    char* ret = dest;
    while (n > 0 && *src) {
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *dest = '\0';
    return ret;
}

/**
 * Find the first occurrence of a character in a string
 * 
 * @param str The string to search
 * @param c The character to find
 * 
 * @return A pointer to the first occurrence of the character
*/
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == c) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

/**
 * Get the length of a string
 * 
 * @param str The string to get the length of
 * 
 * @return The length of the string
*/
size_t strlen(const char* str) {
    size_t len = 0;
    while (*str) {
        str++;
        len++;
    }
    return len;
}

/**
 * Convert a string to an integer
 * 
 * @param str The string to convert
 * 
 * @return The integer value of the string
*/
int atoi(const char* str) {
    int result = 0;
    while (*str) {
        result = result * 10 + *str - '0';
        str++;
    }
    return result;
}

/**
 * Copy a string
 * 
 * @param dest The destination string
 * @param src The source string
 * 
 * @return The destination string
*/
char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
}

/**
 * Concatenate two strings
 * 
 * @param dest The destination string
 * @param src The source string
 * 
 * @return The destination string
 */
char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
}

size_t strspn(const char *str, const char *delim) {
    size_t len = 0;
    while(*str && strchr(delim, *str++)) len++;
    return len;
}

size_t strcspn(const char *str, const char *delim) {
    size_t len = 0;
    while(*str && !strchr(delim, *str++)) len++;
    return len;
}

char *strtok(char *str, const char *delim) {
    static char* p = 0;
    if(str) p = str;
    else if(!p) return 0;
    str = p + strspn(p, delim);
    p = str + strcspn(str, delim);
    if(p == str) return p = 0;
    p = *p ? *p = 0, p + 1 : 0;
    return str;
}

char *strrchr(const char *str, int c) {
    char *last_occurrence = NULL;
    while (*str) {
        if (*str == c)
            last_occurrence = (char*)str;
        str++;
    }
    return last_occurrence;
}
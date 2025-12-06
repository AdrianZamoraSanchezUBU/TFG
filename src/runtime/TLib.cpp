#include "Runtime.h"
#include <cstdarg>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Transforms a int to its string value.
 * @param x Int to convert
 * @return string buffer
 */
extern "C" char *toString(int x) {
    char *buf = (char *)malloc(12);
    sprintf(buf, "%d", x);
    return buf;
}

/**
 * @brief Prints the parameters to the std out.
 * @param first At least the first string to print.
 * @param ... More string parameters to print, might be none.
 */
extern "C" void print(const char *first, ...) {
    va_list args;
    va_start(args, first);

    const char *s = first;

    while (s != NULL) {
        fputs(s, stdout);
        s = va_arg(args, const char *);
    }

    fputc('\n', stdout);
    fflush(stdout);
    va_end(args);
}
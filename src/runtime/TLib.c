#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Transforms a int to its string value.
 * @param x Int to convert 
 * @return string buffer
 */
char* toString(int x) {
    char* buf = malloc(12);
    sprintf(buf, "%d", x);
    return buf;
}

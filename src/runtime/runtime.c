#include <stdio.h>
#include <stdlib.h>

/**
 * Main LLVM caller
 */
int mainLLVM(void);
int main(int argc, char** argv) {
    int ret = mainLLVM();
    return ret;
}
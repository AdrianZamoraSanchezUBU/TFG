#include "Runtime.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

/**
 * Main LLVM caller
 */
extern "C" int mainLLVM(void);
int main(int argc, char **argv) {
    Runtime runtime;

    int ret = mainLLVM();

    runtime.start();

    return ret;
}
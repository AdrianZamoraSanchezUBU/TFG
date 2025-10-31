#include "Compiler.h"

/**
 * @brief Entry point of the compiler.
 *
 * This function performs the complete compilation process.
 *
 * @param argc Number of arguments.
 * @param argv Vector with the arguments.
 * @return Program exit code (0 if everything was successful).
 */
int main(int argc, char *argv[]) {
    // File reading
    CompilerFlags flags;

    // Extract the flags from the argv
    try {
        flags = argvToFlags(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return 1;
    }

    // Creates the compiler with the flags
    Compiler compiler(flags);

    if (!compiler.lex())
        return 1;
    if (!compiler.parse())
        return 1;
    if (!compiler.analyze())
        return 1;
    if (!compiler.generateIR())
        return 1;

    return 0;
}

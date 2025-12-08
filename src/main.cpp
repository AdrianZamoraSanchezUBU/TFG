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
    // Extract the flags from the argv
    CompilerFlags flags;
    try {
        flags = argvToFlags(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return 1;
    }

    // Creates the compiler with the flags
    Compiler compiler(flags);

    try {
        // Compilation process
        compiler.lex();
        compiler.parse();
        compiler.analyze();
        compiler.generateIR();
        compiler.generateObjectCode();
        compiler.linkObjectFile();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

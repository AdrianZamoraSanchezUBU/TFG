#include "Compiler.h"

/**
 * Wrapper function for single compiler phase execution.
 */
template <typename Func> bool runPhase(const char *phaseName, Func &&f) {
    try {
        f();
        return true;
    } catch (const std::exception &e) {
        spdlog::critical("{} exception: {}", phaseName, e.what());
        return false;
    }
}

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
        spdlog::critical("Invalid argument: {}", e.what());
        return 1;
    }

    // Creates the compiler with the flags
    Compiler compiler(flags);

    // Compilation process
    if (!runPhase("Lexer", [&] { compiler.lex(); }))
        return 1;
    if (!runPhase("Parser", [&] { compiler.parse(); }))
        return 1;
    if (!runPhase("Semantic analysis", [&] { compiler.analyze(); }))
        return 1;
    if (!runPhase("IR generation", [&] { compiler.generateIR(); }))
        return 1;

    // If errors are present in the code, the compiler will not try to generate the executable
    if (compiler.getErrorCount() > 0) {
        compiler.printErrors();
        return 1;
    }

    // Final compilation phases, object and executable code generation
    if (!runPhase("Object file generation", [&] { compiler.generateObjectCode(); }))
        return 1;
    if (!runPhase("Linker", [&] { compiler.linkObjectFile(); }))
        return 1;

    return 0;
}
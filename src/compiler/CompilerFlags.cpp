#include "CompilerFlags.h"

std::string readFile(const std::string fileName) {
    std::ifstream testFile(fileName);

    // Opens the file
    if (!testFile.is_open()) {
        // Throw error if couldn't open the file
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    std::ostringstream buffer;
    buffer << testFile.rdbuf(); // Reads the file content
    return buffer.str();
}

CompilerFlags argvToFlags(int argc, char **argv) {
    // Checks for invalid arguments
    if (argc < 2) {
        throw std::invalid_argument("No input file specified.");
    }

    CompilerFlags flags;
    flags.inputFile = argv[1];

    // Arguments check
    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "-o") {
                if (i + 1 >= argc)
                    throw std::invalid_argument("Missing output file after -o");
                flags.inputFile = argv[++i];
            } else if (arg == "--visualizeAST") {
                flags.visualizeAST = true;
            } else if (arg == "--debug") {
                flags.debug = true;
            } else {
                throw std::invalid_argument("Unknown argument: " + arg);
            }
        }
    }

    // Default outputFile name
    if (flags.outputFile.empty()) {
        flags.outputFile = "out.o";
    }

    return flags;
}
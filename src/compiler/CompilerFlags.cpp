#include "CompilerFlags.h"

std::string readFile(const std::string &fileName) {
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
    argparse::ArgumentParser program("TCompiler");

    // Defined arguments
    program.add_argument("input").help("Input source file.");

    program.add_argument("-o", "--output").help("Output object file").default_value(std::string("out"));

    program.add_argument("--visualizeAST")
        .help("Generates a AST visualization -pdf file")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--debug")
        .help("Shows debug data about all the compiler phases.")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-IR").help("Generates a LLVM IR file.").default_value(std::string(""));

    // Is the arguments are invalid throws invalid_argument exception
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &e) {
        throw std::invalid_argument(program.help().str());
    }

    // Setting the flags with the parsed arguments
    CompilerFlags flags;
    flags.inputFile = program.get<std::string>("input");
    flags.outputFile = program.get<std::string>("--output");
    flags.visualizeAST = program.get<bool>("--visualizeAST");
    flags.debug = program.get<bool>("--debug");
    flags.generateIRFile = program.get<std::string>("-IR");

    return flags;
}
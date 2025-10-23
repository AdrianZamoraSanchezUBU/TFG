/**
 * @file CompilerFlags.h
 * @brief Defines the compiler flags and a helper to parse them.
 *
 * @author Adrián Zamora Sánche
 */
#pragma once
#include <fstream>
#include <sstream>
#include <stdexcept>

struct CompilerFlags {
    std::string inputFile;
    std::string outputFile;
    bool visualizeAST;
    bool debug;
};

/**
 * @brief Returns the text contained in a file.
 *
 * Returns the text contained in a file or throws a runtime_error
 * if the file cannot be opened for reading.
 *
 * @param fileName Name of the file to be read.
 * @return String with the file's content.
 * @throw std::runtime_error If the file does not exist.
 */
std::string readFile(const std::string);

/**
 * @brief Parses command-line arguments, supporting both simple flags and key-value pairs.
 *
 * Supported forms:
 *   - `-o output.o`      -> Output file
 *   - `fileName.T`       -> Input file
 *   - `--visualizeAST`   -> Sets visualize flag to true
 *   - `--debug`          -> Sets the debug flag to true
 *
 * @param argc Argument count.
 * @param argv Argument values.
 * @throws std::invalid_argument
 * @return Compiler flags.
 */
CompilerFlags argvToFlags(int, char **);
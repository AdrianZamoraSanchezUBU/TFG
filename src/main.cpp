#include <stdio.h>
#include <stdlib.h>

#include <iostream>

// ANTLR
#include "LexerErrorListener.h"
#include "ParserErrorListener.h"
#include "TLexer.h"

// AST
#include "ASTBuilder.h"

// LLVM
#include <llvm/Support/raw_ostream.h>

#include "IRGenerator.h"

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
std::string readFile(const std::string fileName) {
    std::ifstream testFile(fileName);
    // Opens the file
    if (!testFile.is_open()) {
        // Throw error if couldnt open the file
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    std::ostringstream buffer;
    buffer << testFile.rdbuf(); // Reads all the file content
    return buffer.str();
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
    // Arguments check
    if (argc < 1) {
        std::cerr << "A file name is needed" << std::endl;
    }

    // File reading
    std::string fileContent = readFile(argv[1]);

    /* Lexical analysis */
    antlr4::ANTLRInputStream input(fileContent);
    TLexer lexer(&input);

    // Modified lexer error listener
    lexer.removeErrorListeners();
    lexer.addErrorListener(new LexerErrorListener());

    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    // TODO: remove debug
    /*
    for (auto token : tokens.getTokens()) {
            std::cout << token->toString() << std::endl;
    }
    */

    /* Parsing process */
    TParser parser(&tokens);

    // Modified parser error listener
    parser.removeErrorListeners();
    parser.addErrorListener(new ParserErrorListener());

    TParser::ProgramContext *programCtx = parser.program();

    // TODO: remove debug
    // std::cout << programCtx->toStringTree(&parser) << std::endl;

    /* AST build process */
    std::vector<std::unique_ptr<ASTNode>> ast;

    try {
        bool visualizeFlag = false;

        // Checks for the --vAST flag to determine whether to generate an AST visualization
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--vAST") {
                visualizeFlag = true;
                break;
            }
        }

        // AST generation process
        ASTBuilder builder(visualizeFlag);
        ast = builder.visit(programCtx);
    } catch (const std::exception &e) {
        std::cerr << "Error during AST build process: " << e.what() << '\n';
    }

    // Codegen
    IRGenerator IRgen;
    llvm::Value *result = ast[0]->accept(IRgen);

    // TODO: remove debug
    CodegenContext &ctx = IRgen.getContext();
    ctx.IRBuilder.CreateRet(result);
    ctx.IRModule->print(llvm::outs(), nullptr);

    return 0;
}

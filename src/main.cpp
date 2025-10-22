#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

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
    std::unique_ptr<ASTNode> astRoot;

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
        astRoot = builder.visit(programCtx);

        // xelatex compilation and cleaning
        if (std::system("xelatex --version > /dev/null 2>&1") == 0 && visualizeFlag) {
            // Compiles the .tex file
            std::string texName = "AST";
            std::string command = "xelatex -interaction=nonstopmode " + texName + ".tex > /dev/null 2>&1";
            std::system(command.c_str());

            // Clean the .log .aux and .tex files
            std::string cleanupCmd = "rm -f " + texName + ".log " + texName + ".aux " + texName + ".tex";
            std::system(cleanupCmd.c_str());
        }
    } catch (const std::exception &e) {
        std::cerr << "Error during AST build process: " << e.what() << '\n';
    }

    // Codegen
    IRGenerator IRgen;
    llvm::Value *entryBlock = nullptr;
    CodegenContext &ctx = IRgen.getContext();
    if (auto *block = dynamic_cast<CodeBlockNode *>(astRoot.get())) {
        llvm::Value *F = block->accept(IRgen);

        llvm::Value *result = block->getStmt(0)->accept(IRgen);

        llvm::Function *func = llvm::cast<llvm::Function>(F);
        llvm::BasicBlock *BB = &func->getEntryBlock();
        ctx.IRBuilder.SetInsertPoint(BB);
        ctx.IRBuilder.CreateRet(result);
    }

    // TODO: remove debug
    ctx.IRModule->print(llvm::outs(), nullptr);

    return 0;
}

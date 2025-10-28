#include "Compiler.h"

bool Compiler::lex() {
    // Reads the input file and generates the lexer
    std::string fileContent = readFile(flags.inputFile);
    inputStream = std::make_unique<antlr4::ANTLRInputStream>(fileContent);

    lexer = std::make_unique<TLexer>(inputStream.get());

    // Custom lexer error listener
    lexerErrorListener = std::make_shared<LexerErrorListener>();
    lexer->removeErrorListeners();
    lexer->addErrorListener(lexerErrorListener.get());

    // Checks for error
    if (lexerErrorListener->hasErrors()) {
        std::cerr << "Error in lexical analysis phase" << std::endl;
        return false;
    }

    // Fill the token list
    tokenList = std::make_shared<antlr4::CommonTokenStream>(lexer.get());
    tokenList->fill();

    // Shows the tokens if the debug flag is set to true
    if (flags.debug) {
        // Printing the input text
        std::cout << "****** COMPILER INPUT ****** \n" << fileContent << "\n" << std::endl;

        // Printing tokens
        std::cout << "****** TOKEN LIST ******" << std::endl;
        for (auto token : tokenList->getTokens()) {
            std::cout << token->toString() << std::endl;
        }
        std::cout << std::endl;
    }

    return true;
}

bool Compiler::parse() {
    TParser parser(tokenList.get());

    // Custom lexer error listener
    parserErrorListener = std::make_shared<ParserErrorListener>();
    lexer->removeErrorListeners();
    lexer->addErrorListener(parserErrorListener.get());

    // Checks for error
    if (parserErrorListener->hasErrors()) {
        std::cerr << "Error in syntactic analysis phase" << std::endl;
        return false;
    }

    TParser::ProgramContext *programCtx = parser.program();

    try {
        // AST generation process
        ASTBuilder builder(flags.visualizeAST || flags.debug);
        ast = builder.visit(programCtx);

        // xelatex compilation and cleaning
        if (std::system("xelatex --version > /dev/null 2>&1") == 0 && (flags.visualizeAST || flags.debug)) {
            // Compiles the .tex file
            std::string texName = "AST";
            std::string command = "xelatex -interaction=nonstopmode " + texName + ".tex > /dev/null 2>&1";

            if (std::system(command.c_str()) == 0 && flags.debug) {
                std::cout << "****** AST VISUALIZATION GENERATED AT: ./AST.pdf ******\n" << std::endl;
            }

            // Clean the .log .aux and .tex files
            std::string cleanupCmd = "rm -f " + texName + ".log " + texName + ".aux " + texName + ".tex";
            std::system(cleanupCmd.c_str());
        }
    } catch (const std::exception &e) {
        std::cerr << "Error during AST build process: " << e.what() << '\n';
        return false;
    }

    return true;
}

bool Compiler::analyze() {
    getAST()->accept(*analyser);

    if (flags.debug) {
        std::cout << "****** SYMBOL TABLE ******" << std::endl;
        analyser->printSymbolTable();
        std::cout << std::endl;
    }

    return true;
}

bool Compiler::generateIR() {
    CodegenContext &ctx = IRgen.get()->getContext();

    // FIXME: REMOVE WHEN FUNCTIONS ARE IMPLEMENTED
    if (auto *block = dynamic_cast<CodeBlockNode *>(getAST())) {
        llvm::Value *result;

        for (int i = 0; i < block->getStmtCount(); i++) {
            result = block->getStmt(i)->accept(*IRgen);
        }

        ctx.IRBuilder.CreateRet(result);
    }

    // Debug IR print
    if (flags.debug) {
        std::cout << "****** GENERATED LLVM IR ******" << std::endl;
        ctx.IRModule->print(llvm::outs(), nullptr);
    }

    return true;
}
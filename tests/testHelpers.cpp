#include "testHelpers.h"

bool test(const std::string &fileName, ASTNode *expectedAST, const std::string &expectedIR) {
    CompilerFlags flags;
    flags.inputFile = fileName;

    Compiler compiler(flags);

    /* Lexer test */
    try {
        compiler.lex();
    } catch (const std::exception &e) {
        ADD_FAILURE() << "Lexical analysis phase failed: " << e.what();
    }

    /* Parser test */
    try {
        compiler.parse();

        if (!compiler.getAST()->equals(expectedAST)) {
            compiler.getAST()->print();
            std::cout << std::endl;
            expectedAST->print();
        }

        EXPECT_TRUE(compiler.getAST()->equals(expectedAST));
    } catch (const std::exception &e) {
        ADD_FAILURE() << "Syntactic analysis phase failed: " << e.what();
    }

    /* Semantic visitor test */
    try {
        compiler.analyze();
    } catch (const std::exception &e) {
        ADD_FAILURE() << "Semantic analysis phase failed: " << e.what();
    }

    /* IR test */
    try {
        compiler.generateIR();

        std::string rawIRString;
        llvm::raw_string_ostream rso(rawIRString);
        compiler.getIRContext().IRModule->print(rso, nullptr);
        rso.flush();

        compiler.getIRContext().IRModule->print(rso, nullptr);

        std::regex regex(expectedIR, std::regex::extended);

        EXPECT_TRUE(std::regex_search(rawIRString, regex));
    } catch (const std::exception &e) {
        ADD_FAILURE() << "IR generation phase failed: " << e.what();
    }

    return true;
}

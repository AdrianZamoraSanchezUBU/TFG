#include <gtest/gtest.h>

#include <fstream>
#include <sstream>

#include "AST.h"
#include "ASTBuilder.h"
#include "IRGenerator.h"
#include "ParserErrorListener.h"
#include "TLexer.h"
#include "TParser.h"
#include "antlr4-runtime.h"
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

/// copydoc readFile
std::string readFile(const std::string fileName) {
    std::ifstream testFile(fileName);

    if (!testFile.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    std::ostringstream buffer;
    buffer << testFile.rdbuf(); // Reads the file

    return buffer.str();
}

/**
 * @brief Compares two strings in IR format
 * @param a first string
 * @param b second string
 * @param ctx LLVM context
 * @return `true` if the strings are equal, `false` otherwise.
 */
bool IRsAreEqual(const std::string &a, const std::string &b, llvm::LLVMContext &ctx) {
    llvm::SMDiagnostic err;

    // Parse first string
    std::unique_ptr<llvm::Module> modA = llvm::parseAssemblyString(a, err, ctx);
    if (!modA) {
        std::cerr << "Error parsing first IR string:\n";
        err.print("IRTest", llvm::errs());
        return false;
    }

    // Parse second string
    std::unique_ptr<llvm::Module> modB = llvm::parseAssemblyString(b, err, ctx);
    if (!modB) {
        std::cerr << "Error parsing second IR string:\n";
        err.print("IRTest", llvm::errs());
        return false;
    }

    // Serialize both strings as raw ostream
    std::string strA, strB;
    llvm::raw_string_ostream rsoA(strA);
    llvm::raw_string_ostream rsoB(strB);

    modA->print(rsoA, nullptr);
    modB->print(rsoB, nullptr);

    // Shows both IR strings
    std::cout << "=== IR A ===\n" << rsoA.str() << "\n";
    std::cout << "=== IR B ===\n" << rsoB.str() << "\n";

    return rsoA.str() == rsoB.str();
}

/**
 * @brief Runs a test of the IRGenerator for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return Generated IR code.
 */
bool testIR(const std::string &fileName, std::string expectedIR) {
    std::string fileContent = readFile(fileName);

    // Lexing process
    antlr4::ANTLRInputStream input(fileContent);
    TLexer lexer(&input);

    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    // Parsing process
    TParser parser(&tokens);

    parser.removeErrorListeners();
    auto *errorListener = new ParserErrorListener();
    parser.addErrorListener(errorListener);

    TParser::ProgramContext *tree = parser.program();

    /* AST build process */
    ASTBuilder builder;
    std::vector<std::unique_ptr<ASTNode>> ast;

    try {
        ast = builder.visit(tree);
    } catch (const std::exception &e) {
        std::cerr << "Error during AST build process: " << e.what() << '\n';
    }

    // Codegen
    IRGenerator IRgen;
    llvm::Value *result = ast[0]->accept(IRgen);

    CodegenContext &ctx = IRgen.getContext();
    ctx.IRBuilder.CreateRet(result);

    std::string rawIRString;
    llvm::raw_string_ostream rso(rawIRString);
    ctx.IRModule->print(rso, nullptr);

    return IRsAreEqual(rso.str(), expectedIR, ctx.IRContext);
}

TEST(IRTest, aritmeticExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicArithmeticExpr.T";

    /* Expected result */
    // clang-format off
    std::string expectedIR = R"(
        ; ModuleID = 'program' 
        source_filename = "program"

        define i32 @main() {
        entry:
            ret i32 4
        }
    )"
    ; // clang-format on

    // Result of the IR comparation
    bool result = testIR(fileName, expectedIR);

    EXPECT_TRUE(result);
}

// FIXME: Error i1 value doesn't match function result type i32
TEST(IRTest, numericLogicalExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicLogicalExpr.T";

    /* Expected result */
    // clang-format off
    std::string expectedIR = R"(
        ; ModuleID = 'program' 
        source_filename = "program"

        define i1 @main() {
        entry:
            ret i1 true
        }
    )"
    ; // clang-format on

    // Result of the IR comparation
    bool result = testIR(fileName, expectedIR);

    EXPECT_TRUE(result);
}

TEST(IRTest, stringLogicalExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "stringLogicalExprEQ.T";

    /* Expected result */
    // clang-format off
    std::string expectedIR = R"(
       ; ModuleID = 'program'
        source_filename = "program"

        @.str = private unnamed_addr constant [9 x i8] c"\22string\22\00", align 1
        @.str.1 = private unnamed_addr constant [9 x i8] c"\22string\22\00", align 1

        define i32 @main() {
        entry:
        %0 = call i32 @strcmp(ptr @.str, ptr @.str.1)
        %1 = icmp eq i32 %0, 0
        %2 = select i1 %1, i32 1, i32 0
        ret i32 %2
        }

        declare i32 @strcmp(ptr, ptr)
    )"
    ; // clang-format on

    // Result of the IR comparation
    bool result = testIR(fileName, expectedIR);

    EXPECT_TRUE(result);
}

/**
 * @brief Runs the tests associated with the ASTBuilder.
 * @see ASTNode
 * @see ASTBuilder
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

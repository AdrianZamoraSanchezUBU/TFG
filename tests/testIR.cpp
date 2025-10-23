#include <gtest/gtest.h>

#include <fstream>
#include <sstream>

#include "Compiler.h"
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

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

    std::cout << rsoA.str() << std::endl;
    std::cout << rsoB.str() << std::endl;

    return rsoA.str() == rsoB.str();
}

/**
 * @brief Runs a test of the IRGenerator for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return Generated IR code.
 */
bool testIR(const std::string &fileName, std::string expectedIR) {
    CompilerFlags flags;
    flags.inputFile = fileName;

    Compiler compiler(flags);

    if (!compiler.lex())
        return false;
    if (!compiler.parse())
        return false;
    if (!compiler.analyze())
        return false;
    if (!compiler.generateIR())
        return false;

    std::string rawIRString;
    llvm::raw_string_ostream rso(rawIRString);

    compiler.getIRContext().IRModule->print(rso, nullptr);

    return IRsAreEqual(rso.str(), expectedIR, compiler.getIRContext().IRContext);
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

// FIXME: Error i1 value doesn't match function result type i32
TEST(IRTest, stringLogicalExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "stringLogicalExprEQ.T";

    /* Expected result */
    // clang-format off
    std::string expectedIR = R"(
        ; ModuleID = 'program'
        source_filename = "program"

        define i32 @main() {
        entry:
            ret i1 true
        }
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

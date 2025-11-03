#include <gtest/gtest.h>

#include "Compiler.h"

/**
 * @brief Runs a test of the lexer for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return `true` if the lexer accepted the code, `false` otherwise.
 */
bool runLexerTest(const std::string &fileName) {
    CompilerFlags flags;
    flags.inputFile = fileName;

    Compiler compiler(flags);

    try {
        compiler.lex();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

TEST(LexerTest, BasicArithmeticExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicArithmeticExpr.T";

    EXPECT_TRUE(runLexerTest(fileName)) << "Lexer fault at analizing the file: " << fileName;
}

TEST(LexerTest, BasicLogicalExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicLogicalExpr.T";

    EXPECT_TRUE(runLexerTest(fileName)) << "Lexer fault at analizing the file:" << fileName;
}

TEST(LexerTest, ComplexExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";

    EXPECT_TRUE(runLexerTest(fileName)) << "Lexer fault at analizing the file:" << fileName;
}

/**
 * @brief Runs the tests associated with the lexer.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

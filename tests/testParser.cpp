#include <gtest/gtest.h>

#include "Compiler.h"

/**
 * @brief Runs a test of the parser for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return `true` if the parser accepted the code, `false` otherwise.
 */
bool runParserTest(const std::string &fileName) {
    CompilerFlags flags;
    flags.inputFile = fileName;

    Compiler compiler(flags);

    if (!compiler.lex())
        return false;
    if (!compiler.parse())
        return false;

    return true;
}

TEST(ParserTest, BasicArithmeticExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicArithmeticExpr.T";

    EXPECT_TRUE(runParserTest(fileName)) << "Parser fault at analizing the file: " << fileName;
}

TEST(ParserTest, BasicLogicalExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicLogicalExpr.T";

    EXPECT_TRUE(runParserTest(fileName)) << "Parser fault at analizing the file:" << fileName;
}

TEST(ParserTest, ComplexExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";

    EXPECT_TRUE(runParserTest(fileName)) << "Parser fault at analizing the file:" << fileName;
}

/**
 * @brief Runs the tests associated with the parser.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

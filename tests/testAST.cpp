#include <gtest/gtest.h>

#include <fstream>
#include <sstream>

#include "Compiler.h"

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
 * @brief Runs a test of the ASTBuilder for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return Root node of the generated AST.
 */
ASTNode *runASTTest(const std::string &fileName, Compiler &compiler) {
    compiler.lex();
    compiler.parse();
    return compiler.getAST();
}

TEST(ASTTest, Expr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";

    CompilerFlags flags;
    flags.inputFile = fileName;
    Compiler compiler(flags);

    ASTNode *result = runASTTest(fileName, compiler);

    /* Expected result */
    auto leftOperation =
        std::make_unique<BinaryExprNode>("+", std::make_unique<LiteralNode>(3, SupportedTypes::TYPE_INT),
                                         std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT));

    auto rightOperation =
        std::make_unique<BinaryExprNode>("+", std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT),
                                         std::make_unique<LiteralNode>(1, SupportedTypes::TYPE_INT));

    std::unique_ptr<BinaryExprNode> rootExpr =
        std::make_unique<BinaryExprNode>("-", std::move(leftOperation), std::move(rightOperation));

    std::vector<std::unique_ptr<ASTNode>> statements;

    statements.push_back(std::move(rootExpr));

    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    EXPECT_TRUE(result->equals(root.get())) << "Unexpected AST result with: " << fileName;
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

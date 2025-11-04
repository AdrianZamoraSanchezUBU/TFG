#include "testHelpers.h"

TEST(exprTest, basicArithmeticExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicArithmeticExpr.T";

    /* Expected AST */
    auto lhs = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto rhs = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto expr = std::make_unique<BinaryExprNode>("+", std::move(lhs), std::move(rhs));
    auto ret = std::make_unique<ReturnNode>(std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> functionBlockStmts;
    functionBlockStmts.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionBlockStmts));

    std::vector<std::unique_ptr<ASTNode>> params;
    auto func = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_INT, std::move(functionBlock));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(func));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(i32 4)";

    test(fileName, root.get(), expectedIR);
}

TEST(exprTest, basicLogicalExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "basicLogicalExpr.T";

    /* Expected AST */
    auto lhs = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto rhs = std::make_unique<LiteralNode>(3, SupportedTypes::TYPE_INT);
    auto expr = std::make_unique<BinaryExprNode>("<", std::move(lhs), std::move(rhs));
    auto ret = std::make_unique<ReturnNode>(std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> functionBlockStmts;
    functionBlockStmts.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionBlockStmts));

    std::vector<std::unique_ptr<ASTNode>> params;
    auto func = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_INT, std::move(functionBlock));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(func));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(i1 true)";

    test(fileName, root.get(), expectedIR);
}

TEST(exprTest, stringLogicalExprEQ) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "stringLogicalExprEQ.T";

    /* Expected AST */
    auto lhs = std::make_unique<LiteralNode>("\"string\"", SupportedTypes::TYPE_STRING);
    auto rhs = std::make_unique<LiteralNode>("\"string\"", SupportedTypes::TYPE_STRING);
    auto expr = std::make_unique<BinaryExprNode>("==", std::move(lhs), std::move(rhs));
    expr.get()->setType(SupportedTypes::TYPE_STRING);
    auto ret = std::make_unique<ReturnNode>(std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> functionBlockStmts;
    functionBlockStmts.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionBlockStmts));

    std::vector<std::unique_ptr<ASTNode>> params;
    auto func = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_BOOL, std::move(functionBlock));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(func));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(i1 true)";

    test(fileName, root.get(), expectedIR);
}

TEST(exprTest, stringLogicalExprNQ) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "stringLogicalExprNQ.T";

    /* Expected AST */
    auto lhs = std::make_unique<LiteralNode>("\"string1\"", SupportedTypes::TYPE_STRING);
    auto rhs = std::make_unique<LiteralNode>("\"string2\"", SupportedTypes::TYPE_STRING);
    auto expr = std::make_unique<BinaryExprNode>("!=", std::move(lhs), std::move(rhs));
    expr.get()->setType(SupportedTypes::TYPE_STRING);
    auto ret = std::make_unique<ReturnNode>(std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> functionBlockStmts;
    functionBlockStmts.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionBlockStmts));

    std::vector<std::unique_ptr<ASTNode>> params;
    auto func = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_BOOL, std::move(functionBlock));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(func));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(i1 true)";

    test(fileName, root.get(), expectedIR);
}

TEST(exprTest, complexExpr) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";

    /* Expected AST */
    auto leftOperation =
        std::make_unique<BinaryExprNode>("+", std::make_unique<LiteralNode>(3, SupportedTypes::TYPE_INT),
                                         std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT));

    auto rightOperation =
        std::make_unique<BinaryExprNode>("+", std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT),
                                         std::make_unique<LiteralNode>(1, SupportedTypes::TYPE_INT));

    std::unique_ptr<BinaryExprNode> expr =
        std::make_unique<BinaryExprNode>("-", std::move(leftOperation), std::move(rightOperation));
    auto ret = std::make_unique<ReturnNode>(std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> functionBlockStmts;
    functionBlockStmts.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionBlockStmts));

    std::vector<std::unique_ptr<ASTNode>> params;
    auto func = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_INT, std::move(functionBlock));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(func));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(i32 2)";

    test(fileName, root.get(), expectedIR);
}

/**
 * @brief Runs the tests associated with the parser.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
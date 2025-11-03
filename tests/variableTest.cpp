#include "testHelpers.h"

TEST(variableTest, variableDec) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableDec.T";

    /* Expected AST */
    auto dec = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "a");

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(%a = alloca i32, align 4)";

    test(fileName, root.get(), expectedIR);
}

TEST(variableTest, variableAssign) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableAssign.T";

    /* Expected AST */
    auto lit = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);

    auto dec = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "a");
    auto assign = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_VOID, "a", std::move(lit));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    statements.push_back(std::move(assign));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(store i32 2, ptr %a, align 4)";

    test(fileName, root.get(), expectedIR);
}

TEST(variableTest, variableDecAssign) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableDecAssign.T";

    /* Expected AST */
    auto assing = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);

    auto dec = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_INT, "a", std::move(assing));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(store i32 2, ptr %a, align 4)";

    test(fileName, root.get(), expectedIR);
}

TEST(variableTest, variableCall) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableCall.T";

    /* Expected AST */
    auto lit1 = std::make_unique<LiteralNode>(1, SupportedTypes::TYPE_INT);
    auto assignX = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_INT, "x", std::move(lit1));

    auto decY = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "y");

    // x + 2
    auto refX = std::make_unique<VariableRefNode>("x");
    auto lit2 = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto expr = std::make_unique<BinaryExprNode>("+", std::move(refX), std::move(lit2));

    // y = x + 2
    auto assignY = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_VOID, "y", std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(assignX));
    statements.push_back(std::move(decY));
    statements.push_back(std::move(assignY));

    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(%addtmp = add i32 %x1, 2)";

    test(fileName, root.get(), expectedIR);
}

/**
 * @brief Runs the tests associated with the parser.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
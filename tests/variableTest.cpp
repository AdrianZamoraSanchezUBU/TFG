#include "testHelpers.h"

TEST(variableTest, variableDec) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableDec.T";

    /* Expected AST */
    auto dec = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "a");
    auto ret = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(0, SupportedTypes::TYPE_INT));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    statements.push_back(std::move(ret));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(%a_ptr = alloca i32, align 4)";

    test(fileName, root.get(), expectedIR);
}

TEST(variableTest, variableAssign) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableAssign.T";

    /* Expected AST */
    auto lit = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);

    auto dec = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "a");
    auto assign = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_VOID, "a", std::move(lit));

    auto ret = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(0, SupportedTypes::TYPE_INT));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    statements.push_back(std::move(assign));
    statements.push_back(std::move(ret));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(store i32 2, ptr %a_ptr, align 4)";

    test(fileName, root.get(), expectedIR);
}

TEST(variableTest, variableDecAssign) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableDecAssign.T";

    /* Expected AST */
    auto assing = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);

    auto dec = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_INT, "a", std::move(assing));

    auto ret = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(0, SupportedTypes::TYPE_INT));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    statements.push_back(std::move(ret));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(store i32 2, ptr %a_ptr, align 4)";

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

    // Return
    auto ret = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(0, SupportedTypes::TYPE_INT));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(assignX));
    statements.push_back(std::move(decY));
    statements.push_back(std::move(assignY));
    statements.push_back(std::move(ret));

    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(%addtmp = add i32 %x_val, 2)";

    test(fileName, root.get(), expectedIR);
}

TEST(variableTest, variableTimeType) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "variableTime.T";

    std::vector<std::unique_ptr<ASTNode>> statements;
    auto varDec = std::make_unique<VariableDecNode>(Type(SupportedTypes::TYPE_TIME), "timeVar");
    statements.push_back(std::move(varDec));

    auto timeLit = std::make_unique<TimeLiteralNode>(5.2, TimeStamp::TYPE_SEC);
    auto varAssign =
        std::make_unique<VariableAssignNode>(Type(SupportedTypes::TYPE_TIME), "timeVar", std::move(timeLit));
    statements.push_back(std::move(varAssign));

    auto retNode = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(0, Type(SupportedTypes::TYPE_INT)));
    statements.push_back(std::move(retNode));

    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(%timeVar_ptr = alloca float, align 4)");
    regexpr.push_back(R"(store float 5.2)");

    test(fileName, regexpr);
}

TEST(variableTest, variableShadowing) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "varShadowing.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(ptr %a_val, ptr null)");
    regexpr.push_back(R"(ptr %a_val1, ptr null)"); // This a_val1 shadows a_val

    test(fileName, regexpr);
}

/**
 * @brief Runs the tests associated with variables.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
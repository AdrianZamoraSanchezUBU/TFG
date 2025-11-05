#include "testHelpers.h"

TEST(functionTest, functionDec) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionDec.T";

    /* Expected AST */
    std::vector<SupportedTypes> params;
    params.push_back(SupportedTypes::TYPE_INT);

    auto dec = std::make_unique<FunctionDecNode>("foo", params, SupportedTypes::TYPE_INT);

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(dec));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(declare i32 @foo)";

    test(fileName, root.get(), expectedIR);
}

TEST(functionTest, functionDef) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionDef.T";

    /* Expected AST */
    std::vector<std::unique_ptr<ASTNode>> params;
    auto variableDec = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "x");
    params.push_back(std::move(variableDec));

    auto rhs = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto lhs = std::make_unique<VariableRefNode>("x");
    auto expr = std::make_unique<BinaryExprNode>("*", std::move(lhs), std::move(rhs));
    auto ret = std::make_unique<ReturnNode>(std::move(expr));

    std::vector<std::unique_ptr<ASTNode>> functionStatements;
    functionStatements.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionStatements));

    auto def = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_INT, std::move(functionBlock));

    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(def));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(ret i32 %multmp)";

    test(fileName, root.get(), expectedIR);
}

TEST(functionTest, functionCall) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionCall.T";

    /* Function def */
    std::vector<std::unique_ptr<ASTNode>> params;
    auto variableDec = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "x");
    params.push_back(std::move(variableDec));

    auto ref = std::make_unique<VariableRefNode>("x");
    auto ret = std::make_unique<ReturnNode>(std::move(ref));

    std::vector<std::unique_ptr<ASTNode>> functionStatements;
    functionStatements.push_back(std::move(ret));
    auto functionBlock = std::make_unique<CodeBlockNode>(std::move(functionStatements));

    auto def = std::make_unique<FunctionDefNode>("foo", params, SupportedTypes::TYPE_INT, std::move(functionBlock));

    /* Variable (function call param) dec */
    auto val = std::make_unique<LiteralNode>(1, SupportedTypes::TYPE_INT);
    auto varRef = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_INT, "param", std::move(val));

    /* Function call */
    std::vector<std::unique_ptr<ASTNode>> callParams;
    auto param = std::make_unique<VariableRefNode>("param");
    callParams.push_back(std::move(param));
    auto call = std::make_unique<FunctionCallNode>("foo", std::move(callParams));

    /* Main program block */
    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(def));
    statements.push_back(std::move(varRef));
    statements.push_back(std::move(call));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::string expectedIR = R"(call i32 @foo)";

    test(fileName, root.get(), expectedIR);
}

/**
 * @brief Runs the tests associated with functions.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#include "testHelpers.h"

TEST(ifElseTest, ifStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "if.T";

    /* Expected AST */
    // Condition
    auto lhs = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto rhs = std::make_unique<LiteralNode>(3, SupportedTypes::TYPE_INT);
    auto expr = std::make_unique<BinaryExprNode>("<", std::move(lhs), std::move(rhs));

    // Block of code
    auto ifRetVal = std::make_unique<LiteralNode>(1, SupportedTypes::TYPE_INT);
    auto programRetVal = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto retIf = std::make_unique<ReturnNode>(std::move(ifRetVal));
    auto retProgram = std::make_unique<ReturnNode>(std::move(programRetVal));
    std::vector<std::unique_ptr<ASTNode>> ifBlockStatements;
    ifBlockStatements.push_back(std::move(retIf));
    auto ifBlock = std::make_unique<CodeBlockNode>(std::move(ifBlockStatements));

    // If statement node
    auto ifNode = std::make_unique<IfNode>(std::move(expr), std::move(ifBlock));

    // Program block
    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(ifNode));
    statements.push_back(std::move(retProgram));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 true, label %then, label %endif");

    test(fileName, root.get(), regexpr);
}

TEST(ifElseTest, ifElseStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "else.T";

    /* Expected AST */
    // If Condition
    auto lhs = std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT);
    auto rhs = std::make_unique<LiteralNode>(3, SupportedTypes::TYPE_INT);
    auto expr = std::make_unique<BinaryExprNode>("<", std::move(rhs), std::move(lhs));

    // Else statement node
    auto varB = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "b");
    std::vector<std::unique_ptr<ASTNode>> elseBlockStatements;
    elseBlockStatements.push_back(std::move(varB));
    auto elseBlock = std::make_unique<CodeBlockNode>(std::move(elseBlockStatements));
    auto elseNode = std::make_unique<ElseNode>(std::move(elseBlock));

    // If statement node
    auto varA = std::make_unique<VariableDecNode>(SupportedTypes::TYPE_INT, "a");
    std::vector<std::unique_ptr<ASTNode>> ifBlockStatements;
    ifBlockStatements.push_back(std::move(varA));
    auto ifBlock = std::make_unique<CodeBlockNode>(std::move(ifBlockStatements));
    auto ifNode = std::make_unique<IfNode>(std::move(expr), std::move(ifBlock), std::move(elseNode));

    auto retVal = std::make_unique<LiteralNode>(0, SupportedTypes::TYPE_INT);
    auto ret = std::make_unique<ReturnNode>(std::move(retVal));

    // Program block
    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(ifNode));
    statements.push_back(std::move(ret));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 false, label %then, label %else");
    regexpr.push_back("then:");
    regexpr.push_back("else:");
    regexpr.push_back("br label %endif");
    regexpr.push_back("endif:                                            ; preds = %else, %then");

    test(fileName, root.get(), regexpr);
}

TEST(ifElseTest, nestedIf) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "nestedIf.T";

    // Var dec + def
    auto varB = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_INT, "b",
                                                     std::make_unique<LiteralNode>(3, SupportedTypes::TYPE_INT));
    auto varA = std::make_unique<VariableAssignNode>(SupportedTypes::TYPE_INT, "a",
                                                     std::make_unique<LiteralNode>(2, SupportedTypes::TYPE_INT));

    // 1st and 2nd conditions
    auto cond1 = std::make_unique<BinaryExprNode>("<", std::make_unique<VariableRefNode>("a"),
                                                  std::make_unique<VariableRefNode>("b"));
    auto cond2 = std::make_unique<BinaryExprNode>("==", std::make_unique<VariableRefNode>("a"),
                                                  std::make_unique<VariableRefNode>("b"));

    // 2st If statement node
    std::vector<std::unique_ptr<ASTNode>> ifBlockStatements2;
    auto ret2 = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(0, SupportedTypes::TYPE_INT));
    ifBlockStatements2.push_back(std::move(ret2));
    auto ifBlock2 = std::make_unique<CodeBlockNode>(std::move(ifBlockStatements2));
    auto ifNode2 = std::make_unique<IfNode>(std::move(cond2), std::move(ifBlock2));

    // 1st If statement node
    auto ret1 = std::make_unique<ReturnNode>(std::make_unique<LiteralNode>(1, SupportedTypes::TYPE_INT));
    std::vector<std::unique_ptr<ASTNode>> ifBlockStatements1;
    ifBlockStatements1.push_back(std::move(ifNode2));
    ifBlockStatements1.push_back(std::move(ret1));
    auto ifBlock1 = std::make_unique<CodeBlockNode>(std::move(ifBlockStatements1));
    auto ifNode1 = std::make_unique<IfNode>(std::move(cond1), std::move(ifBlock1));

    // Program block
    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::move(varA));
    statements.push_back(std::move(varB));
    statements.push_back(std::move(ifNode1));
    auto root = std::make_unique<CodeBlockNode>(std::move(statements));

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 %lttmp, label %then, label %endif");
    regexpr.push_back("br i1 %eqtmp, label %then1, label %endif2");
    regexpr.push_back("then:                                             ; preds = %entry");
    regexpr.push_back("endif:                                            ; preds = %entry");
    regexpr.push_back("then1:                                            ; preds = %then");
    regexpr.push_back("endif2:                                           ; preds = %then");

    test(fileName, root.get(), regexpr);
}

TEST(loopTest, elseIfStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "elseif.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 false, label %then, label %else");
    regexpr.push_back("then:                                             ; preds = %entry");
    regexpr.push_back("else:                                             ; preds = %entry");
    regexpr.push_back("endif:                                            ; preds = %endif3, %then");
    regexpr.push_back("then1:                                            ; preds = %else");
    regexpr.push_back("else2:                                            ; preds = %else");
    regexpr.push_back("endif3:                                           ; preds = %endif6, %then1");
    regexpr.push_back("then4:                                            ; preds = %else2");
    regexpr.push_back("else5:                                            ; preds = %else2");
    regexpr.push_back("endif6:                                           ; preds = %else5, %then4");

    test(fileName, regexpr);
}

/**
 * @brief Runs the tests associated with the if-else statement.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
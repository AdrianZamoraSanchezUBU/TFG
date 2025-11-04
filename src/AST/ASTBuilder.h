/**
 * @file ASTBuilder.h
 * @brief Contains the definition of a custom AST builder with a visittor
 * pattern.
 *
 * @author Adrián Zamora Sánchez
 * @see AST.h
 */

#pragma once
#include "SupportedTypes.h"
#include "TParser.h"

// Forward declaration
class ASTNode;

/**
 * @class ASTBuilder
 * @brief Generates the AST using the visitor pattern.
 *
 * This class visits the program contexts in an orderly manner, generating a
 * node for each visit and thereby constructing the complete AST.
 *
 * @see ASTNode
 */
class ASTBuilder {
    bool visualizeFlag;

  public:
    /**
     * @brief ASTBuilder basic constructor
     */
    explicit ASTBuilder() : visualizeFlag(false) {}

    /**
     * @brief ASTBuilder constructor for a visualizeFlag value assignation
     */
    explicit ASTBuilder(bool flagValue) : visualizeFlag(flagValue) {}

    /**
     * @brief Visits the program and returns the nodes it contains.
     * @param ctx Base program context.
     * @return Vector with the program's nodes.
     */
    std::unique_ptr<ASTNode> visit(TParser::ProgramContext *ctx);

    /**
     * @brief Visits the program main block.
     * @param ctx Main code block context.
     * @return Vector with the program's nodes.
     */
    std::unique_ptr<ASTNode> visit(TParser::ProgramMainBlockContext *ctx);

    /**
     * @brief Visits the context of a block of code.
     * @param ctx Block context.
     * @return AST node representing a block.
     */
    std::unique_ptr<ASTNode> visit(TParser::BlockContext *ctx);

    /**
     * @brief Visits the context of a statement.
     * @param ctx Statement context.
     * @return AST node representing that statement.
     */
    std::unique_ptr<ASTNode> visit(TParser::StmtContext *ctx);

    /**
     * @brief Visits the context of arithmetic and logical expressions.
     *
     * Visits the context of arithmetic and logical expressions,
     * serving as a dispatcher to visit the different types of expressions.
     *
     * @param ctx Expression context.
     * @return AST node associated with the expression.
     */
    std::unique_ptr<ASTNode> visit(TParser::ExprContext *ctx);

    /**
     * @brief Visits the context of an expression operator.
     * @param ctx Operator context.
     * @return AST node associated with an operator.
     */
    std::unique_ptr<ASTNode> visit(TParser::OperandExprContext *ctx);

    /**
     * @brief Visits the context of an arithmetic expression.
     * @param ctx Arithmetic expression context.
     * @return AST node associated with the expression.
     */
    std::unique_ptr<ASTNode> visit(TParser::ArithmeticExprContext *ctx);

    /**
     * @brief Visits the context of a logical expression.
     * @param ctx Logical expression context.
     * @return AST node associated with the expression.
     */
    std::unique_ptr<ASTNode> visit(TParser::LogicalExprContext *ctx);

    /**
     * @brief Visits the context of a literal.
     * @param ctx Literal context.
     * @return Node representing a literal value in the code.
     */
    std::unique_ptr<ASTNode> visit(TParser::LiteralContext *ctx);

    /**
     * @brief Visits the context of a variable declaration.
     * @param ctx Variable declaration context.
     * @return AST node associated the variable declaration.
     */
    std::unique_ptr<ASTNode> visit(TParser::VariableDecContext *ctx);

    /**
     * @brief Visits the context of a variable assignment.
     * @param ctx Variable assignment context.
     * @return AST node associated the variable assignment.
     */
    std::unique_ptr<ASTNode> visit(TParser::VariableAssignContext *ctx);

    /**
     * @brief Visits the context of params of a function.
     * @param ctx Variable assignment context.
     * @return AST node associated the params.
     */
    std::vector<SupportedTypes> visit(TParser::ParamsContext *ctx);

    /**
     * @brief Visit a function definition node.
     * @param ctx Context of a return statement.
     * @return AST node associated with the function definition statement.
     */
    std::unique_ptr<ASTNode> visit(TParser::FunctionDefinitionContext *ctx);

    /**
     * @brief Visit a function declaration node.
     * @param ctx Context of a return statement.
     * @return AST node associated with the function declaration statement.
     */
    std::unique_ptr<ASTNode> visit(TParser::FunctionDeclarationContext *ctx);

    /**
     * @brief Visit a function call node.
     * @param ctx Context of a return statement.
     * @return AST node associated with the function call statement.
     */
    std::unique_ptr<ASTNode> visit(TParser::FunctionCallContext *ctx);

    /**
     * @brief Visits the context of a return statement.
     * @param ctx Context of a return statement.
     * @return AST node associated with the return statement.
     */
    std::unique_ptr<ASTNode> visit(TParser::Return_stmtContext *ctx);

    /**
     * @brief Visits the type context of a variable declaration.
     * @param ctx Type context in a variable declaration.
     * @return SupportedTypes A supported type representing the token value.
     */
    static SupportedTypes visit(TParser::TypeContext *ctx);
};

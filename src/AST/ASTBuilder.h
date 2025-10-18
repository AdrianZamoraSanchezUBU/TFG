/**
 * @file ASTBuilder.h
 * @brief Contains the definition of a custom AST builder with a visittor
 * pattern.
 *
 * @author Adrián Zamora Sánchez
 * @see AST.h
 */

#pragma once
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
    bool visualizeFlag = false;

  public:
    /**
     * @brief ASTBuilder basic constructor
     */
    ASTBuilder() : visualizeFlag(false) {}

    /**
     * @brief ASTBuilder constructor for a visualizeFlag value assignation
     */
    ASTBuilder(bool flagValue) : visualizeFlag(flagValue) {}

    /**
     * @brief Visits the program and returns the nodes it contains.
     * @param ctx Base program context.
     * @return Vector with the program's nodes.
     */
    std::vector<std::unique_ptr<ASTNode>> visit(TParser::ProgramContext *ctx);

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
};

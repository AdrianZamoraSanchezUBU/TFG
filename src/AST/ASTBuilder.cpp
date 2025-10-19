#include "ASTBuilder.h"

#include <fstream>
#include <vector>

#include "AST.h"

/**
 * @brief Adds the TickZ styles for the AST visualization.
 * @return string with the styles.
 * @todo
 */
std::string includeTikzStyles() {
    return R"(
\tikzset{
    programNode/.style={
        draw, 
        circle, 
        fill=purple!20!white,
        outer sep=2pt, 
        minimum size=2.5em,
        align=center
    },
    binaryNode/.style={
        draw, 
        diamond, 
        fill=yellow!25!white, 
        aspect=2, 
        minimum size=3em,
        align=center
    },
    literalNode/.style={
        draw, 
        ellipse, 
        fill=green!20!white, 
        minimum width=3.5em,
        minimum height=2.5em, 
        align=center
    }
})";
}

/**
 * @brief Adds a header for the .tex output file when visualizing the AST.
 * @return string with the header.
 */
std::string includeTexHeader() {
    return R"(
\documentclass[border=5mm]{standalone}

\usepackage{bbding}
\usepackage{tikz,tikz-qtree,tikz-qtree-compat}
\usepackage{amssymb}
\usepackage{forest}

\usetikzlibrary{shapes}
\usetikzlibrary{positioning}
)" + includeTikzStyles() +
           R"(
\newcommand{\sep}{-.1mm}

\begin{document}

\begin{forest}
    for tree={
    align=center,
    parent anchor=south,
    child anchor=north,
    s sep=30pt,
    l sep=10pt,
    inner sep=1pt,
    }
[Program,programNode)";
}

// Root of the program
std::vector<std::unique_ptr<ASTNode>> ASTBuilder::visit(TParser::ProgramContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> AST;

    // AST visualization argument check
    if (visualizeFlag) {
        std::ofstream texFile("AST.tex");

        // Tex file header and styles
        texFile << includeTexHeader() << std::endl;
    }

    // Visits all the stmts
    for (int i = 0; i < ctx->stmt().size(); i++) {
        AST.push_back(visit(ctx->stmt(i)));
    }

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Tex footer file
        texFile << "]" << std::endl;
        texFile << R"(\end{forest})" << std::endl;
        texFile << R"(\end{document})" << std::endl;

        texFile.close();
    }

    return AST;
}

// Dispatcher for STMT
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::StmtContext *ctx) {
    if (ctx->expr()) {
        return visit(ctx->expr());
    }

    throw std::runtime_error("Not a valid stmt");
}

/* Expr management*/

// Dispatcher for EXPR
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ExprContext *ctx) {
    if (auto aritmeticCtx = dynamic_cast<TParser::ArithmeticExprContext *>(ctx)) {
        return visit(aritmeticCtx);
    } else if (auto logicalCtx = dynamic_cast<TParser::LogicalExprContext *>(ctx)) {
        return visit(logicalCtx);
    } else if (auto operandCtx = dynamic_cast<TParser::OperandExprContext *>(ctx)) {
        return visit(operandCtx);
    } else if (auto parenCtx = dynamic_cast<TParser::ParenExprContext *>(ctx)) {
        return visit(parenCtx->expr());
    }

    throw std::runtime_error("Not a valid expr");
}

// Arithmetic Expr
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ArithmeticExprContext *ctx) {
    std::string op = ctx->op->getText();

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Node information
        texFile << "[{" << op << "},binaryNode" << std::endl;

        texFile.close();
    }

    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));

    auto exprNode = std::make_unique<BinaryExprNode>(op, std::move(lhs), std::move(rhs));

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Node information
        texFile << "]" << std::endl;

        texFile.close();
    }

    return exprNode;
}

// Logical Expr
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LogicalExprContext *ctx) {
    std::string op = ctx->comparisonOperator()->getText();

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Node information
        texFile << "[{" << op << "},binaryNode" << std::endl;

        texFile.close();
    }

    // Child visits
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));

    auto exprNode = std::make_unique<BinaryExprNode>(op, std::move(lhs), std::move(rhs));

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Node information
        texFile << "]" << std::endl;

        texFile.close();
    }

    return exprNode;
}

/* Smaller components */
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::OperandExprContext *ctx) {
    auto operand = ctx->operand();

    // Checks if the operand is a identifier (variable) or a literal
    if (operand->IDENTIFIER()) {
        return nullptr; // TODO add identifiers
    }

    if (operand->literal()) {
        return visit(operand->literal());
    }

    throw std::runtime_error("Not a valid operand");
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LiteralContext *ctx) {
    // Checks for a number or float context
    if (ctx->NUMBER_LITERAL()) {
        int value = stoi(ctx->NUMBER_LITERAL()->getText());
        auto node = std::make_unique<LiteralNode>(value);

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << ctx->NUMBER_LITERAL()->getText() << "},literalNode]" << std::endl;

            texFile.close();
        }

        return node;
    }
    if (ctx->FLOAT_LITERAL()) {
        float value = stof(ctx->FLOAT_LITERAL()->getText());
        auto node = std::make_unique<LiteralNode>(value);

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << ctx->FLOAT_LITERAL()->getText() << "},literalNode]" << std::endl;

            texFile.close();
        }

        return node;
    }
    if (ctx->STRING_LITERAL()) {
        auto node = std::make_unique<LiteralNode>(ctx->STRING_LITERAL()->getText());

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << ctx->STRING_LITERAL()->getText() << "},literalNode]" << std::endl;

            texFile.close();
        }

        return node;
    }

    throw std::runtime_error("Not a valid literal");
}

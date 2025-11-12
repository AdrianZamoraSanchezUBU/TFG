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
    },
    variableDecNode/.style={
        draw, 
        rectangle, 
        fill=cyan!15!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },
    varRefNode/.style={
        draw, 
        rectangle, 
        fill=green!10!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },variableAssignNode/.style={
        draw, 
        rectangle, 
        fill=violet!20!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },returnNode/.style={
        draw, 
        rectangle, 
        fill=violet, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },functionDecNode/.style={
        draw, 
        regular polygon, 
        regular polygon sides=5, 
        fill=teal!25!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },functionDefNode/.style={
        draw, 
        regular polygon, 
        regular polygon sides=6, 
        fill=teal!25!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },functionCallNode/.style={
        draw, 
        regular polygon,
        regular polygon sides=3, 
        fill=brown!20!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },paramsNode/.style={
        draw, 
        rectangle, 
        fill=orange!10!white, 
        minimum size=2.5em,
        minimum width=2.5em,
        minimum height=2em, 
        align=center
    },IfNode/.style={
        draw, 
        rectangle, 
        fill=violet!10!white, 
        minimum size=2.7em,
        minimum width=3.2em,
        minimum height=2.2em, 
        align=center
    },ElseNode/.style={
        draw, 
        rectangle, 
        fill=violet!10!white, 
        minimum size=2.7em,
        minimum width=3.2em,
        minimum height=2.2em, 
        align=center
    },LoopNode/.style={
        draw, 
        regular polygon,
        regular polygon sides=3, 
        fill=red!20!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
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

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ProgramContext *ctx) {

    // AST visualization argument check
    if (visualizeFlag) {
        std::ofstream texFile("AST.tex");

        // Tex file header and styles
        texFile << includeTexHeader() << std::endl;
    }

    auto entryBlock = visit(ctx->programMainBlock());

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Tex footer file
        texFile << "]" << std::endl;
        texFile << R"(\end{forest})" << std::endl;
        texFile << R"(\end{document})" << std::endl;

        texFile.close();
    }

    return entryBlock;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ProgramMainBlockContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> stmt;

    // Visits all the stmts
    for (auto child : ctx->children) {
        if (auto stmtCtx = dynamic_cast<TParser::StmtContext *>(child)) {
            stmt.push_back(visit(stmtCtx));
        } else if (auto retCtx = dynamic_cast<TParser::Return_stmtContext *>(child)) {
            stmt.push_back(visit(retCtx));
        }
    }

    auto codeBlock = std::make_unique<CodeBlockNode>(std::move(stmt));

    return codeBlock;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::BlockContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> stmt;

    // Visits all the stmts
    for (auto child : ctx->children) {
        if (auto stmtCtx = dynamic_cast<TParser::StmtContext *>(child)) {
            stmt.push_back(visit(stmtCtx));
        } else if (auto retCtx = dynamic_cast<TParser::Return_stmtContext *>(child)) {
            stmt.push_back(visit(retCtx));
        }
    }

    auto codeBlock = std::make_unique<CodeBlockNode>(std::move(stmt));

    return codeBlock;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::StmtContext *ctx) {
    if (ctx->expr()) {
        return visit(ctx->expr());
    } else if (ctx->variableDec()) {
        return visit(ctx->variableDec());
    } else if (ctx->variableAssign()) {
        return visit(ctx->variableAssign());
    } else if (ctx->functionDefinition()) {
        return visit(ctx->functionDefinition());
    } else if (ctx->functionDeclaration()) {
        return visit(ctx->functionDeclaration());
    } else if (ctx->functionCall()) {
        return visit(ctx->functionCall());
    } else if (ctx->if_()) {
        return visit(ctx->if_());
    } else if (ctx->loop()) {
        return visit(ctx->loop());
    }

    throw std::runtime_error("Not a valid stmt");
}

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

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::OperandExprContext *ctx) {
    auto operand = ctx->operand();

    // Checks if the operand is a identifier (variable) or a literal
    if (operand->IDENTIFIER()) {
        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << operand->IDENTIFIER()->getText() << "},varRefNode]" << std::endl;

            texFile.close();
        }
        return std::make_unique<VariableRefNode>(operand->IDENTIFIER()->getText());
    }

    if (operand->literal()) {
        return visit(operand->literal());
    }

    if (operand->functionCall()) {
        return visit(operand->functionCall());
    }

    throw std::runtime_error("Not a valid operand");
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LiteralContext *ctx) {
    // Checks for a number or float context
    if (ctx->NUMBER_LITERAL()) {
        int value = stoi(ctx->NUMBER_LITERAL()->getText());
        auto node = std::make_unique<LiteralNode>(value, SupportedTypes::TYPE_INT);

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << ctx->NUMBER_LITERAL()->getText() << "},literalNode]" << std::endl;

            texFile.close();
        }

        return node;
    }

    if (ctx->boolean_literal()) {
        bool value = false;

        if (ctx->boolean_literal()->getText() == "true") {
            value = true;
        }

        auto node = std::make_unique<LiteralNode>(value, SupportedTypes::TYPE_BOOL);

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << ctx->boolean_literal()->getText() << "},literalNode]" << std::endl;

            texFile.close();
        }

        return node;
    }

    if (ctx->FLOAT_LITERAL()) {
        float value = stof(ctx->FLOAT_LITERAL()->getText());
        auto node = std::make_unique<LiteralNode>(value, SupportedTypes::TYPE_FLOAT);

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);

            // Node information
            texFile << "[{" << ctx->FLOAT_LITERAL()->getText() << "},literalNode]" << std::endl;

            texFile.close();
        }

        return node;
    }

    if (ctx->STRING_LITERAL()) {
        auto node = std::make_unique<LiteralNode>(ctx->STRING_LITERAL()->getText(), SupportedTypes::TYPE_STRING);

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

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::VariableDecContext *ctx) {
    SupportedTypes type = visit(ctx->type());

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);

        // Node information
        texFile << "[{" << typeToString(type) << " " << ctx->IDENTIFIER()->getText() << "},variableDecNode]"
                << std::endl;

        texFile.close();
    }

    return std::make_unique<VariableDecNode>(type, ctx->IDENTIFIER()->getText());
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::VariableAssignContext *ctx) {
    std::string varName;
    SupportedTypes type;
    std::string typeString;
    std::unique_ptr<ASTNode> assign;

    // If this is a variable declaration + assignment visit the type
    if (ctx->variableDec()) {
        varName = ctx->variableDec()->IDENTIFIER()->getText();
        type = visit(ctx->variableDec()->type());
        typeString = ctx->variableDec()->type()->getText();
    } else {
        varName = ctx->IDENTIFIER()->getText();
        type = SupportedTypes::TYPE_VOID;
    }

    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);

        if (type == SupportedTypes::TYPE_VOID) {
            texFile << "[{" << varName << "},variableAssignNode" << std::endl;
        } else {
            texFile << "[{" << typeString << " " << varName << "},variableAssignNode" << std::endl;
        }

        texFile.close();
    }

    // Visits the expr that gives this variable its value
    assign = visit(ctx->expr());

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "]" << std::endl;
    }

    return std::make_unique<VariableAssignNode>(type, varName, std::move(assign));
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::FunctionDefinitionContext *ctx) {
    std::string id = ctx->IDENTIFIER()->getText();
    SupportedTypes type = visit(ctx->type());

    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);

        texFile << "[{" << id << "},functionDefNode" << std::endl;

        texFile.close();
    }

    std::vector<std::unique_ptr<ASTNode>> params;

    // Visits all the types
    if (ctx->params() != nullptr && !ctx->params()->isEmpty()) {
        for (int i = 0; i < ctx->params()->IDENTIFIER().size(); i++) {
            std::string id = ctx->params()->IDENTIFIER(i)->getText();
            SupportedTypes type = visit(ctx->params()->type(i));

            params.emplace_back(std::make_unique<VariableDecNode>(type, id));
        }
    }

    auto codeBlock = visit(ctx->block());
    auto raw = codeBlock.release();
    auto functionScope = dynamic_cast<CodeBlockNode *>(raw);
    std::unique_ptr<CodeBlockNode> codeBlockPtr(functionScope);

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "]" << std::endl;
    }

    return std::make_unique<FunctionDefNode>(id, params, type, std::move(codeBlockPtr));
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::FunctionDeclarationContext *ctx) {
    std::string id = ctx->IDENTIFIER()->getText();
    SupportedTypes type = visit(ctx->type());

    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);

        texFile << "[{" << id << "},functionDecNode]" << std::endl;

        texFile.close();
    }

    std::vector<SupportedTypes> params;
    if (ctx->params() != nullptr && !ctx->params()->isEmpty()) {
        params = visit(ctx->params());
    }

    return std::make_unique<FunctionDecNode>(id, params, type);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::FunctionCallContext *ctx) {
    std::string id = ctx->IDENTIFIER()->getText();
    std::vector<std::unique_ptr<ASTNode>> params;

    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);

        texFile << "[" << id << ",functionCallNode" << std::endl;

        texFile.close();
    }

    // Visits all the types
    for (int i = 0; i < ctx->expr().size(); i++) {
        if (visualizeFlag) {
            // Node information
            std::ofstream texFile("AST.tex", std::ios::app);

            texFile << "[" << id << " params ,paramsNode" << std::endl;

            texFile.close();
        }

        params.emplace_back(visit(ctx->expr(i)));

        if (visualizeFlag) {
            // Node information
            std::ofstream texFile("AST.tex", std::ios::app);

            texFile << "]" << std::endl;

            texFile.close();
        }
    }

    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);

        texFile << "]" << std::endl;

        texFile.close();
    }

    return std::make_unique<FunctionCallNode>(id, std::move(params));
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::Return_stmtContext *ctx) {
    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);

        texFile << "[return,returnNode" << std::endl;

        texFile.close();
    }

    // Visits the expr that gives this variable its value
    std::unique_ptr<ASTNode> retVal;
    if (ctx->expr()) {
        retVal = visit(ctx->expr());
    }

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "]" << std::endl;
    }

    // Returns a value or void
    if (ctx->expr()) {
        return std::make_unique<ReturnNode>(std::move(retVal));
    } else {
        return std::make_unique<ReturnNode>();
    }
}

std::vector<SupportedTypes> ASTBuilder::visit(TParser::ParamsContext *ctx) {
    std::vector<SupportedTypes> params;

    // Visits all the types
    for (int i = 0; i < ctx->type().size(); i++) {
        params.emplace_back(visit(ctx->type(i)));
    }

    return params;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::IfContext *ctx) {
    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "[IF,IfNode" << std::endl;
        texFile.close();
    }

    // Expr for entering the if code block
    auto expr = visit(ctx->expr());

    // Block of code in the if statement
    auto block = visit(ctx->block());
    auto ifBlock = std::unique_ptr<CodeBlockNode>(static_cast<CodeBlockNode *>(block.release()));

    std::unique_ptr<ASTNode> ifNode;

    // Vistis a else statement if there is one present
    if (ctx->else_()) {
        auto elseStmt = visit(ctx->else_());
        ifNode = std::make_unique<IfNode>(std::move(expr), std::move(ifBlock), std::move(elseStmt));
    } else {
        ifNode = std::make_unique<IfNode>(std::move(expr), std::move(ifBlock));
    }

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "]" << std::endl;
    }

    return std::move(ifNode);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ElseContext *ctx) {
    if (visualizeFlag) {
        // Node information
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "[ELSE,ElseNode" << std::endl;
        texFile.close();
    }

    std::unique_ptr<ASTNode> node;

    // Vistis a nested else if statement
    if (ctx->if_()) {
        auto ifStmt = visit(ctx->if_());
        node = std::make_unique<ElseNode>(std::move(ifStmt));
    }

    // Vistis the else block
    if (ctx->block()) {
        auto block = visit(ctx->block());
        node = std::make_unique<ElseNode>(std::move(block));
    }

    if (visualizeFlag) {
        std::ofstream texFile("AST.tex", std::ios::app);
        texFile << "]" << std::endl;
        texFile.close();
    }

    return node;
}

SupportedTypes ASTBuilder::visit(TParser::TypeContext *ctx) {
    // Type dispatch from tokens to Supported types
    if (ctx->TYPE_INT())
        return SupportedTypes::TYPE_INT;
    if (ctx->TYPE_FLOAT())
        return SupportedTypes::TYPE_FLOAT;
    if (ctx->TYPE_CHAR())
        return SupportedTypes::TYPE_CHAR;
    if (ctx->TYPE_STRING())
        return SupportedTypes::TYPE_STRING;
    if (ctx->TYPE_BOOLEAN())
        return SupportedTypes::TYPE_BOOL;
    if (ctx->TYPE_VOID())
        return SupportedTypes::TYPE_VOID;

    throw std::runtime_error("Not a valid type");
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LoopBlockContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> stmt;

    // Visits all the stmts
    for (auto child : ctx->children) {
        if (auto stmtCtx = dynamic_cast<TParser::StmtContext *>(child)) {
            stmt.push_back(visit(stmtCtx));
        } else if (auto retCtx = dynamic_cast<TParser::Return_stmtContext *>(child)) {
            stmt.push_back(visit(retCtx));
        } else if (auto terminal = dynamic_cast<antlr4::tree::TerminalNode *>(child)) {
            int tokenType = terminal->getSymbol()->getType();

            if (tokenType == TParser::CONTINUE || tokenType == TParser::BREAK) {
                if (visualizeFlag) {
                    // Node information
                    std::ofstream texFile("AST.tex", std::ios::app);
                    texFile << "[" << terminal->getText() << ", returnNode]" << std::endl;
                    texFile.close();
                }
                stmt.push_back(std::make_unique<LoopControlStatementNode>(terminal->getText()));
            }
        }
    }

    auto codeBlock = std::make_unique<CodeBlockNode>(std::move(stmt));

    return codeBlock;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LoopContext *ctx) {
    if (ctx->WHILE()) {
        if (visualizeFlag) {
            // Node information
            std::ofstream texFile("AST.tex", std::ios::app);
            texFile << "[WHILE,LoopNode" << std::endl;
            texFile.close();
        }

        auto expr = visit(ctx->expr());

        auto block = visit(ctx->loopBlock());
        auto whileBlock = std::unique_ptr<CodeBlockNode>(static_cast<CodeBlockNode *>(block.release()));

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);
            texFile << "]" << std::endl;
            texFile.close();
        }

        return std::make_unique<WhileNode>(std::move(expr), std::move(whileBlock));
    }

    if (ctx->FOR()) {
        if (visualizeFlag) {
            // Node information
            std::ofstream texFile("AST.tex", std::ios::app);
            texFile << "[FOR,LoopNode" << std::endl;
            texFile.close();
        }
        auto def = visit(ctx->variableAssign(0));
        auto condition = visit(ctx->expr());
        auto assign = visit(ctx->variableAssign(1));

        auto block = visit(ctx->loopBlock());
        auto forBlock = std::unique_ptr<CodeBlockNode>(static_cast<CodeBlockNode *>(block.release()));

        if (visualizeFlag) {
            std::ofstream texFile("AST.tex", std::ios::app);
            texFile << "]" << std::endl;
            texFile.close();
        }

        return std::make_unique<ForNode>(std::move(def), std::move(condition), std::move(assign), std::move(forBlock));
    }

    return nullptr;
};
#include <iostream>
#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"
#include "TParserBaseVisitor.h"
#include "AST.h"
#include "ASTBuilder.h"

// Root of the program
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ProgramContext* ctx) {
	// TODO Visit the list of stmt		
    return visit(ctx->stmt(0));	
}

// Dispatcher for STMT
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::StmtContext* ctx) {
	
	if (ctx->expr()) {
        return visit(ctx->expr());
    }
		
    return nullptr;	
}

/* Expr management*/

// Dispatcher for EXPR
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ExprContext *ctx) {
	if (auto aritmeticCtx = dynamic_cast<TParser::ArithmeticExprContext*>(ctx)) {
        return visit(aritmeticCtx);
    }
    else if (auto logicalCtx = dynamic_cast<TParser::LogicalExprContext*>(ctx)) {
        return visit(logicalCtx);
    }
    else if (auto operandCtx = dynamic_cast<TParser::OperandExprContext*>(ctx)) {
        return visit(operandCtx);
    }
    else if (auto parenCtx = dynamic_cast<TParser::ParenExprContext*>(ctx)) {
    	return visit(parenCtx->expr());
    }
    
    
   	return nullptr;
}

// Arithmetic Expr
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ArithmeticExprContext *ctx) {
	std::string op = ctx->op->getText();
	std::cout << op << std::endl; // TODO remove
	
	auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));
    
	auto exprNode = std::make_unique<BinaryExprNode>(op, std::move(lhs), std::move(rhs));
	
   	return exprNode;
}

// Logical Expr
std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LogicalExprContext *ctx) {
	std::string op = ctx->comparisonOperator()->getText();
	std::cout << op << std::endl; // TODO remove

	auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));
    
	auto exprNode = std::make_unique<BinaryExprNode>(op, std::move(lhs), std::move(rhs));
	
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

    return nullptr;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LiteralContext *ctx) {
	// Checks for a number or float context
	if(ctx->NUMBER_LITERAL()) {
		std::cout << ctx->NUMBER_LITERAL()->getText() << std::endl; // TODO remove
		int value = stoi(ctx->NUMBER_LITERAL()->getText());
		auto node = std::make_unique<LiteralIntNode>(value);

        return node;
	}if(ctx->FLOAT_LITERAL()) {
		std::cout << ctx->FLOAT_LITERAL()->getText() << std::endl; // TODO remove
		float value = stof(ctx->FLOAT_LITERAL()->getText());
	    auto node = std::make_unique<LiteralFloatNode>(value);

	    return node;
	}

    return nullptr;
}

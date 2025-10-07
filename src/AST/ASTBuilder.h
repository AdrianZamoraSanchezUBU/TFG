#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"
#include "TParserBaseVisitor.h"

class ASTBuilder {
public:
	std::unique_ptr<ASTNode> visit(TParser::ProgramContext* ctx);

	std::unique_ptr<ASTNode> visit(TParser::StmtContext* ctx);
	
	std::unique_ptr<ASTNode> visit(TParser::OperandExprContext *ctx);

	std::unique_ptr<ASTNode> visit(TParser::LiteralContext *ctx);

	std::unique_ptr<ASTNode> visit(TParser::ArithmeticExprContext *ctx);

	std::unique_ptr<ASTNode> visit(TParser::LogicalExprContext *ctx);

	std::unique_ptr<ASTNode> visit(TParser::ExprContext *ctx);
};

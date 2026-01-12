#include "ASTBuilder.h"

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ProgramContext *ctx) {
    auto entryBlock = visit(ctx->programMainBlock());
    return entryBlock;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ProgramMainBlockContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> stmt;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits all the stmts
    for (auto child : ctx->children) {
        if (auto stmtCtx = dynamic_cast<TParser::StmtContext *>(child)) {
            stmt.push_back(visit(stmtCtx));
        } else if (auto retCtx = dynamic_cast<TParser::Return_stmtContext *>(child)) {
            stmt.push_back(visit(retCtx));
            break; // All the code after a return is dead code
        } else if (auto controlStmt = dynamic_cast<TParser::LoopControlStatementContext *>(child)) {
            stmt.push_back(visit(controlStmt));
        }
    }

    return std::make_unique<CodeBlockNode>(std::move(stmt), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::BlockContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> stmt;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits all the stmts
    for (auto child : ctx->children) {
        if (auto stmtCtx = dynamic_cast<TParser::StmtContext *>(child)) {
            stmt.push_back(visit(stmtCtx));
        } else if (auto retCtx = dynamic_cast<TParser::Return_stmtContext *>(child)) {
            stmt.push_back(visit(retCtx));
            break; // All the code after a return is dead code
        } else if (auto controlStmt = dynamic_cast<TParser::LoopControlStatementContext *>(child)) {
            stmt.push_back(visit(controlStmt));
        }
    }

    return std::make_unique<CodeBlockNode>(std::move(stmt), loc);
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
    } else if (ctx->eventDef()) {
        return visit(ctx->eventDef());
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
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));

    return std::make_unique<BinaryExprNode>(op, std::move(lhs), std::move(rhs), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LogicalExprContext *ctx) {
    std::string op = ctx->comparisonOperator()->getText();
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Child visits
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));

    return std::make_unique<BinaryExprNode>(op, std::move(lhs), std::move(rhs), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::OperandExprContext *ctx) {
    auto operand = ctx->operand();
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // The operand is a literal
    if (operand->literal()) {
        return visit(operand->literal());
    }

    // The operand is a function call
    if (operand->functionCall()) {
        return visit(operand->functionCall());
    }

    // The operand is a pointer to a identifier
    if (operand->TYPE_PTR()) {
        return std::make_unique<VariableRefNode>(operand->IDENTIFIER()->getText(), loc, true);
    }

    // The operand is a unary prefix operation
    if (operand->INC() && operand->INC()->getSymbol()->getTokenIndex() == operand->start->getTokenIndex()) {
        return std::make_unique<UnaryOperationNode>(operand->IDENTIFIER()->getText(),
                                                    true, // prefix is true
                                                    "++", // isInc is true
                                                    loc);
    }
    if (operand->DEC() && operand->DEC()->getSymbol()->getTokenIndex() == operand->start->getTokenIndex()) {
        return std::make_unique<UnaryOperationNode>(operand->IDENTIFIER()->getText(),
                                                    true, // prefix is true
                                                    "--", // isInc is false
                                                    loc);
    }

    // The operand is a unary postfix operation
    if (operand->IDENTIFIER()) {
        if (operand->INC()) {
            return std::make_unique<UnaryOperationNode>(operand->IDENTIFIER()->getText(),
                                                        false, // prefix is false
                                                        "++",  // isInc is true
                                                        loc);
        }
        if (operand->DEC()) {
            return std::make_unique<UnaryOperationNode>(operand->IDENTIFIER()->getText(),
                                                        false, // prefix is false
                                                        "--",  // isInc is false
                                                        loc);
        }

        // Simple variable
        return std::make_unique<VariableRefNode>(operand->IDENTIFIER()->getText(), loc, false);
    }

    throw std::runtime_error("Not a valid operand");
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LiteralContext *ctx) {
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    bool isNegative = false;
    if (ctx->MINUS()) {
        isNegative = true;
    }

    // Checks for a number or float context
    if (ctx->NUMBER_LITERAL()) {
        int value = stoi(ctx->NUMBER_LITERAL()->getText());

        if (isNegative) {
            value = -value;
        }

        return std::make_unique<LiteralNode>(value, Type(SupportedTypes::TYPE_INT), loc);
    }

    if (ctx->boolean_literal()) {
        bool value = false;

        if (ctx->boolean_literal()->getText() == "true") {
            value = true;
        }

        return std::make_unique<LiteralNode>(value, Type(SupportedTypes::TYPE_BOOL), loc);
    }

    if (ctx->FLOAT_LITERAL()) {
        float value = stof(ctx->FLOAT_LITERAL()->getText());

        if (isNegative) {
            value = -value;
        }

        return std::make_unique<LiteralNode>(value, Type(SupportedTypes::TYPE_FLOAT), loc);
    }

    if (ctx->STRING_LITERAL()) {
        return std::make_unique<LiteralNode>(ctx->STRING_LITERAL()->getText(), Type(SupportedTypes::TYPE_STRING), loc);
    }

    if (ctx->time_literal()) {
        return visit(ctx->time_literal());
    }

    throw std::runtime_error("Not a valid literal");
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::Time_literalContext *ctx) {
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Float value of the time type
    float value;

    // Int value of the time type
    if (ctx->NUMBER_LITERAL()) {
        value = std::stof(ctx->NUMBER_LITERAL()->getText());
    } else {
        value = std::stof(ctx->FLOAT_LITERAL()->getText());
    }

    // Time stamp of the time type
    TimeStamp time = visit(ctx->timeStamp());

    return std::make_unique<TimeLiteralNode>(value, time, loc);
}

TimeStamp ASTBuilder::visit(TParser::TimeStampContext *ctx) {
    // Gets the time stamp or throws a runtime error
    if (ctx->TIME_TICK())
        return TimeStamp::TYPE_TICK;
    if (ctx->TIME_SEC())
        return TimeStamp::TYPE_SEC;
    if (ctx->TIME_MIN())
        return TimeStamp::TYPE_MIN;
    if (ctx->TIME_HR())
        return TimeStamp::TYPE_HR;

    throw std::runtime_error("Not a valid time stamp");
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::VariableDecContext *ctx) {
    Type type = visit(ctx->type());
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    return std::make_unique<VariableDecNode>(type, ctx->IDENTIFIER()->getText(), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::VariableAssignContext *ctx) {
    std::string varName;
    Type type;
    std::string typeString;
    std::unique_ptr<ASTNode> assign;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // If this is a variable declaration + assignment visit the type
    if (ctx->variableDec()) {
        varName = ctx->variableDec()->IDENTIFIER()->getText();
        type = visit(ctx->variableDec()->type());
        typeString = ctx->variableDec()->type()->getText();
    } else {
        varName = ctx->IDENTIFIER()->getText();
        type = Type(SupportedTypes::TYPE_VOID);
    }

    // Visits the expr that gives this variable its value
    assign = visit(ctx->expr());

    return std::make_unique<VariableAssignNode>(type, varName, std::move(assign), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::FunctionDefinitionContext *ctx) {
    std::string id = ctx->IDENTIFIER()->getText();
    Type type = visit(ctx->type());
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits all the param types
    std::vector<std::unique_ptr<ASTNode>> params;
    if (ctx->params() != nullptr && !ctx->params()->isEmpty()) {
        for (int i = 0; i < ctx->params()->IDENTIFIER().size(); i++) {
            std::string id = ctx->params()->IDENTIFIER(i)->getText();
            Type type = visit(ctx->params()->paramType(i));
            SourceLocation paramLoc(ctx->params()->getStart()->getLine(),
                                    ctx->params()->getStart()->getCharPositionInLine());

            params.emplace_back(std::make_unique<VariableDecNode>(type, id, paramLoc));
        }
    }

    // Visits the function components
    auto codeBlock = visit(ctx->block());
    auto raw = codeBlock.release();
    auto functionScope = dynamic_cast<CodeBlockNode *>(raw);
    std::unique_ptr<CodeBlockNode> codeBlockPtr(functionScope);

    return std::make_unique<FunctionDefNode>(id, params, type, std::move(codeBlockPtr), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::FunctionDeclarationContext *ctx) {
    std::string id = ctx->IDENTIFIER()->getText();
    Type type = visit(ctx->type());
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    std::vector<Type> params;
    if (ctx->params() != nullptr && !ctx->params()->isEmpty()) {
        params = visit(ctx->params());
    }

    return std::make_unique<FunctionDecNode>(id, params, type, loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::FunctionCallContext *ctx) {
    std::string id = ctx->IDENTIFIER()->getText();
    std::vector<std::unique_ptr<ASTNode>> params;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits all the params
    for (int i = 0; i < ctx->expr().size(); i++) {

        params.emplace_back(visit(ctx->expr(i)));
    }

    return std::make_unique<FunctionCallNode>(id, std::move(params), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::Return_stmtContext *ctx) {
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits the expr that gives this variable its value
    std::unique_ptr<ASTNode> retVal = nullptr;
    if (ctx->expr()) {
        retVal = visit(ctx->expr());
    }

    // Returns a value or void
    return std::make_unique<ReturnNode>(std::move(retVal), loc);
}

std::vector<Type> ASTBuilder::visit(TParser::ParamsContext *ctx) {
    std::vector<Type> params;

    // Visits all the types
    for (int i = 0; i < ctx->paramType().size(); i++) {
        params.emplace_back(visit(ctx->paramType(i)));
    }

    return params;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::IfContext *ctx) {
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Expr for entering the if code block
    auto expr = visit(ctx->expr());

    // Block of code in the if statement
    auto block = visit(ctx->block());
    auto ifBlock = std::unique_ptr<CodeBlockNode>(static_cast<CodeBlockNode *>(block.release()));

    // Vistis a else statement if there is one present
    std::unique_ptr<ASTNode> ifNode;
    if (ctx->else_()) {
        auto elseStmt = visit(ctx->else_());
        ifNode = std::make_unique<IfNode>(std::move(expr), std::move(ifBlock), loc, std::move(elseStmt));
    } else {
        ifNode = std::make_unique<IfNode>(std::move(expr), std::move(ifBlock), loc);
    }

    return std::move(ifNode);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::ElseContext *ctx) {
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Vistis a nested else if statement or the else block
    std::unique_ptr<ASTNode> node;
    if (ctx->if_()) {
        auto ifStmt = visit(ctx->if_());
        node = std::make_unique<ElseNode>(std::move(ifStmt), loc);
    } else if (ctx->block()) {
        auto block = visit(ctx->block());
        node = std::make_unique<ElseNode>(std::move(block), loc);
    }

    return node;
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LoopContext *ctx) {
    std::unique_ptr<ASTNode> loop;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visit the loop
    if (ctx->WHILE()) {
        // Visits the loop components
        auto expr = visit(ctx->expr());
        auto block = visit(ctx->block());
        auto whileBlock = std::unique_ptr<CodeBlockNode>(static_cast<CodeBlockNode *>(block.release()));

        loop = std::make_unique<WhileNode>(std::move(expr), std::move(whileBlock), loc);
    } else if (ctx->FOR()) {
        // Visits the loop components
        auto def = visit(ctx->variableAssign(0));
        auto condition = visit(ctx->expr());
        auto assign = visit(ctx->variableAssign(1));

        // Visits the loop block
        auto block = visit(ctx->block());
        auto forBlock = std::unique_ptr<CodeBlockNode>(static_cast<CodeBlockNode *>(block.release()));

        loop = std::make_unique<ForNode>(std::move(def), std::move(condition), std::move(assign), std::move(forBlock),
                                         loc);
    }

    return loop;
};

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::LoopControlStatementContext *ctx) {
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());
    return std::make_unique<LoopControlStatementNode>(ctx->getText(), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::EventBlockContext *ctx) {
    std::vector<std::unique_ptr<ASTNode>> stmt;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits all the stmts
    for (auto child : ctx->children) {
        if (auto stmtCtx = dynamic_cast<TParser::StmtContext *>(child)) {
            stmt.push_back(visit(stmtCtx));
        } else if (auto exitStmt = dynamic_cast<TParser::ExitStmtContext *>(child)) {
            SourceLocation loc(exitStmt->getStart()->getLine(), exitStmt->getStart()->getCharPositionInLine());
            stmt.push_back(std::make_unique<ExitNode>(exitStmt->IDENTIFIER()->getText(), loc));
        }
    }

    return std::make_unique<CodeBlockNode>(std::move(stmt), loc);
}

std::unique_ptr<ASTNode> ASTBuilder::visit(TParser::EventDefContext *ctx) {
    // Setting the time command
    TimeCommand command = visit(ctx->timeCommand());
    int execLimit = 0;

    // Visit the time block
    auto codeBlock = visit(ctx->eventBlock());
    auto raw = codeBlock.release();
    auto functionScope = dynamic_cast<CodeBlockNode *>(raw);
    std::unique_ptr<CodeBlockNode> codeBlockPtr(functionScope);

    std::unique_ptr<ASTNode> timeNode;
    SourceLocation loc(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine());

    // Visits all the param types
    std::vector<std::unique_ptr<ASTNode>> params;
    if (ctx->params() != nullptr && !ctx->params()->isEmpty()) {
        for (int i = 0; i < ctx->params()->IDENTIFIER().size(); i++) {
            std::string id = ctx->params()->IDENTIFIER(i)->getText();
            Type type = visit(ctx->params()->paramType(i));
            SourceLocation loc(ctx->params()->getStart()->getLine(),
                               ctx->params()->getStart()->getCharPositionInLine());

            params.emplace_back(std::make_unique<VariableDecNode>(type, id, loc));
        }
    }

    // Visits the execution limit of the event
    if (ctx->eventLimitCondition()) {
        execLimit = visit(ctx->eventLimitCondition());
    }

    // Getting the time from a literal or a variable reference
    if (ctx->time_literal()) {
        timeNode = visit(ctx->time_literal());
    } else if (ctx->IDENTIFIER(1)) {
        timeNode = std::make_unique<VariableRefNode>(ctx->IDENTIFIER(1)->getText(), loc);
    }

    return std::make_unique<EventNode>(ctx->IDENTIFIER(0)->getText(), params, command, std::move(timeNode),
                                       std::move(codeBlockPtr), loc, execLimit);
}

int ASTBuilder::visit(TParser::EventLimitConditionContext *ctx) {
    return stoi(ctx->NUMBER_LITERAL()->getText());
}

Type ASTBuilder::visit(TParser::TypeContext *ctx) {
    // Type dispatch from tokens to Supported types
    if (ctx->TYPE_INT())
        return Type(SupportedTypes::TYPE_INT);
    if (ctx->TYPE_FLOAT())
        return Type(SupportedTypes::TYPE_FLOAT);
    if (ctx->TYPE_CHAR())
        return Type(SupportedTypes::TYPE_CHAR);
    if (ctx->TYPE_STRING())
        return Type(SupportedTypes::TYPE_STRING);
    if (ctx->TYPE_BOOLEAN())
        return Type(SupportedTypes::TYPE_BOOL);
    if (ctx->TYPE_VOID())
        return Type(SupportedTypes::TYPE_VOID);
    if (ctx->TYPE_TIME())
        return Type(SupportedTypes::TYPE_TIME);

    throw std::runtime_error("Not a valid type");
}

Type ASTBuilder::visit(TParser::ParamTypeContext *ctx) {
    // Type dispatch from tokens to Supported types
    if (ctx->TYPE_INT())
        return Type(SupportedTypes::TYPE_INT);
    if (ctx->TYPE_FLOAT())
        return Type(SupportedTypes::TYPE_FLOAT);
    if (ctx->TYPE_CHAR())
        return Type(SupportedTypes::TYPE_CHAR);
    if (ctx->TYPE_STRING())
        return Type(SupportedTypes::TYPE_STRING);
    if (ctx->TYPE_BOOLEAN())
        return Type(SupportedTypes::TYPE_BOOL);
    if (ctx->TYPE_VOID())
        return Type(SupportedTypes::TYPE_VOID);
    if (ctx->TYPE_PTR()) {
        Type t = visit(ctx->type());
        return Type(new Type(t));
    }

    throw std::runtime_error("Not a valid parameter type");
}

TimeCommand ASTBuilder::visit(TParser::TimeCommandContext *ctx) {
    // Gets the time command or throws a runtime error
    if (ctx->EVERY())
        return TimeCommand::TIME_EVERY;
    if (ctx->AT())
        return TimeCommand::TIME_AT;
    if (ctx->AFTER())
        return TimeCommand::TIME_AFTER;

    throw std::runtime_error("Not a valid time command");
};
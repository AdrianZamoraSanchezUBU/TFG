#include "SemanticVisitor.h"

namespace llvm {
class Type;
}

void *SemanticVisitor::visit(CodeBlockNode &node) {
    // Checks for a empty block of code
    if (node.getStmtCount() < 1) {
        throw std::runtime_error("Empty block of code");
        return nullptr;
    }

    // Visits all the statements inside the block
    for (int i = 0; i < node.getStmtCount(); i++) {
        node.getStmt(i)->accept(*this);
        if (auto returnBlock = dynamic_cast<CodeBlockNode *>(node.getStmt(i))) {
            break;
        }
    }

    // Exists the scope at the end of the code block
    symtab.exitScope();

    return nullptr;
}

void *SemanticVisitor::visit(LiteralNode &node) {
    return nullptr;
}

void *SemanticVisitor::visit(TimeLiteralNode &node) {
    int time = node.getTime();

    // Unvalid time value
    if (time < 0) {
        throw std::runtime_error("Error in time literal, the time amount cant be negative");
    }

    // Calculating the equivalent in ticks
    switch (node.getTimeStamp()) {
    case TimeStamp::TYPE_TICK:
        return nullptr;
    case TimeStamp::TYPE_SEC:
        node.setValue(time * 10);
        break;
    case TimeStamp::TYPE_MIN:
        node.setValue(time * 10 * 60);
        break;
    case TimeStamp::TYPE_HR:
        node.setValue(time * 10 * 60 * 60);
        break;
    default:
        throw std::runtime_error("Error in time literal, unknown time stamp");
    }

    // The time should now be in tick format
    node.setTime(TimeStamp::TYPE_TICK);
    return nullptr;
}

void *SemanticVisitor::visit(BinaryExprNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();
    SupportedTypes LT, RT;

    /* Propagation of the type */
    node.getLeft()->accept(*this);
    node.getRight()->accept(*this);

    // Getting the type of the literal nodes
    if (auto left = dynamic_cast<LiteralNode *>(node.getLeft())) {
        LT = left->getType();
    }
    if (auto right = dynamic_cast<LiteralNode *>(node.getRight())) {
        RT = right->getType();
    }

    // Getting the type of the time literal nodes
    if (auto left = dynamic_cast<TimeLiteralNode *>(node.getLeft())) {
        LT = SupportedTypes::TYPE_TIME;
    }
    if (auto right = dynamic_cast<TimeLiteralNode *>(node.getRight())) {
        RT = SupportedTypes::TYPE_TIME;
    }

    // Getting the type of the variables
    if (auto left = dynamic_cast<VariableRefNode *>(node.getLeft())) {
        LT = currentScope->getSymbol(left->getValue())->getType();
    }
    if (auto right = dynamic_cast<VariableRefNode *>(node.getRight())) {
        RT = currentScope->getSymbol(right->getValue())->getType();
    }

    // Visiting other expr nodes
    if (auto left = dynamic_cast<BinaryExprNode *>(node.getLeft())) {
        LT = left->getType();
    }
    if (auto right = dynamic_cast<BinaryExprNode *>(node.getRight())) {
        RT = right->getType();
    }

    // Type assign to the expr
    if (LT == RT) {
        node.setType(LT);
    } else {
        throw std::runtime_error("Diferent types in binary experession with left: " + typeToString(LT) +
                                 " and right: " + typeToString(RT));
    }

    return nullptr;
}

void *SemanticVisitor::visit(VariableDecNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    if (currentScope->getSymbol(node.getValue())) {
        throw std::runtime_error("Variable redeclaration error");
    }

    Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
    currentScope->insertSymbol(newSymbol);

    return nullptr;
}

void *SemanticVisitor::visit(VariableAssignNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();
    node.getAssign()->accept(*this);

    /* Checks if the variable was already declarated (only assign) */
    if (currentScope->contains(node.getValue())) {
        Symbol *sym;
        if (currentScope->getSymbol(node.getValue()))
            sym = currentScope->getSymbol(node.getValue());

        // Check for identifier variable status
        if (sym->getCategory() != SymbolCategory::VARIABLE) {
            throw std::runtime_error("Missing declaration for a identifier used in a variable assignment");
        }

        return nullptr;
    }

    /* Type check for variable dec + assign */
    if (auto val = dynamic_cast<BinaryExprNode *>(node.getAssign())) {
        if (val->getType() != node.getType()) {
            return nullptr;
            throw std::runtime_error("Variable assign with incompatible types, expr: " + typeToString(val->getType()) +
                                     " and variable being assign has: " + typeToString(node.getType()));
        }
    }
    if (auto val = dynamic_cast<LiteralNode *>(node.getAssign())) {
        if (val->getType() != node.getType()) {
            return nullptr;
            throw std::runtime_error(
                "Variable assign with incompatible types for value: " + typeToString(val->getType()) +
                " ,to a varianble declarated as: " + typeToString(node.getType()));
        }
    }
    if (auto val = dynamic_cast<TimeLiteralNode *>(node.getAssign())) {
        if (node.getType() != SupportedTypes::TYPE_TIME) {
            return nullptr;
            throw std::runtime_error(
                "Variable assign with incompatible types for value: " + typeToString(SupportedTypes::TYPE_TIME) +
                " ,to a varianble declarated as: " + typeToString(node.getType()));
        }
    }
    if (auto val = dynamic_cast<VariableRefNode *>(node.getAssign())) {
        Symbol sym = *currentScope->getSymbol(val->getValue());

        if (sym.getType() != node.getType()) {
            return nullptr;
            throw std::runtime_error(
                "Variable assign with incompatible types for value: " + typeToString(sym.getType()) +
                " ,to a varianble declarated as: " + typeToString(node.getType()));
        }
    }
    if (auto val = dynamic_cast<FunctionCallNode *>(node.getAssign())) {
        Symbol sym = *currentScope->getSymbol(val->getValue());

        if (sym.getType() != node.getType()) {
            return nullptr;
            throw std::runtime_error(
                "Variable assign with incompatible types for value: " + typeToString(sym.getType()) +
                " ,to a varianble declarated as: " + typeToString(node.getType()));
        }
    }

    // Inserting the variable in the Symbol Table
    Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
    currentScope->insertSymbol(newSymbol);

    return nullptr;
}

void *SemanticVisitor::visit(VariableRefNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    // Checks if the variable was already declarated
    if (currentScope->contains(node.getValue())) {
        // Check for this identifier variable or parameter status
        if (currentScope.get()->getSymbol(node.getValue())) {
            Symbol sym = *currentScope.get()->getSymbol(node.getValue());

            if (sym.getCategory() != SymbolCategory::VARIABLE && sym.getCategory() != SymbolCategory::PARAMETER) {
                throw std::runtime_error("The symbol in use: " + node.getValue() + " is not a variable");
            }
        }
    }

    return nullptr;
}

void *SemanticVisitor::visit(FunctionDecNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    // Inserts the function identifier in the current scope
    Symbol newSymbol(node.getValue(), &node, SymbolCategory::FUNCTION, node.getType());
    newSymbol.setNumParams(node.getParamsCount());
    currentScope->insertSymbol(newSymbol);

    return nullptr;
}

void *SemanticVisitor::visit(FunctionDefNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    // Inserts the function identifier in the current scope
    Symbol newSymbol(node.getValue(), &node, SymbolCategory::FUNCTION, node.getType());
    newSymbol.setNumParams(node.getParamsCount());
    currentScope->insertSymbol(newSymbol);

    // Creates a new scope for this function
    std::shared_ptr<Scope> newScope = symtab.enterScope();

    // Inserting parameters in the function scope
    if (node.getParamsCount() > 0) {
        for (int i = 0; i < node.getParamsCount(); i++) {
            if (auto var = dynamic_cast<VariableDecNode *>(node.getParam(i))) {
                Symbol newSymbol(var->getValue(), var, SymbolCategory::PARAMETER, var->getType());
                newScope->insertSymbol(newSymbol);
            }
        }
    }

    node.getCodeBlock()->accept(*this);

    return nullptr;
}

void *SemanticVisitor::visit(FunctionCallNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();
    int expectedParams = currentScope->getSymbol(node.getValue())->getNumParams();

    if (node.getValue() == "printf" || node.getValue() == "strlen" || node.getValue() == "toString")
        return nullptr;

    if (node.getParamsCount() != expectedParams) {
        throw std::runtime_error("The function " + node.getValue() + " was declared with " +
                                 std::to_string(expectedParams) + " but is being called with " +
                                 std::to_string(node.getParamsCount()));
    }

    // Checking for uses of undefined variable as params
    for (int i = 0; i < node.getParamsCount(); i++) {
        if (auto var = dynamic_cast<VariableRefNode *>(node.getParam(i))) {
            if (!currentScope->contains(var->getValue())) {
                throw std::runtime_error("Missing declaration for a identifier used in a function call: " +
                                         node.getParam(i)->getValue());
            }
        }
    }

    return nullptr;
}

void *SemanticVisitor::visit(ReturnNode &node) {
    // If the return stmt returns a value
    if (node.getStmt()) {
        // Visits the return value
        node.getStmt()->accept(*this);
    }

    return nullptr;
}

void *SemanticVisitor::visit(IfNode &node) {
    // Visits the condition
    if (node.getExpr() != nullptr) {
        node.getExpr()->accept(*this);
    }

    // Visits the code block
    if (node.getCodeBlock() != nullptr) {
        // Enters a new scope
        symtab.enterScope();
        node.getCodeBlock()->accept(*this);
    }

    // Visits the else statement if there is one present
    if (node.getElseStmt() != nullptr) {
        node.getElseStmt()->accept(*this);
    }

    return nullptr;
}

void *SemanticVisitor::visit(ElseNode &node) {
    // When an if stmt is does not create a new scope (the if statement creates its own)
    if (auto stmt = dynamic_cast<IfNode *>(node.getStmt())) {
        node.getStmt()->accept(*this);
    }

    // When in a else block of code, a new scope is generated
    if (auto stmt = dynamic_cast<CodeBlockNode *>(node.getStmt())) {
        symtab.enterScope();
        node.getStmt()->accept(*this);
    }

    return nullptr;
}

void *SemanticVisitor::visit(WhileNode &node) {
    // Visits its components
    node.getExpr()->accept(*this);

    // New scope for this code block
    symtab.enterScope();

    // Controlling to the loop depth
    loopDepth++;
    node.getCodeBlock()->accept(*this);
    loopDepth--;

    return nullptr;
}

void *SemanticVisitor::visit(ForNode &node) {
    // Visits its components
    node.getDef()->accept(*this);
    node.getCondition()->accept(*this);
    node.getAssign()->accept(*this);

    // New scope for this code block
    symtab.enterScope();

    // Controlling to the loop depth
    loopDepth++;
    node.getCodeBlock()->accept(*this);
    loopDepth--;

    return nullptr;
}

void *SemanticVisitor::visit(LoopControlStatementNode &node) {
    // Check for correct use of continue and break statements
    if (loopDepth <= 0) {
        throw std::runtime_error("Error: \"" + node.getValue() + "\" out of a loop block");
    }

    return nullptr;
}

void *SemanticVisitor::visit(EventNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    // Inserts the event identifier in the current scope
    Symbol newSymbol(node.getValue(), &node, SymbolCategory::FUNCTION, SupportedTypes::TYPE_VOID);
    newSymbol.setNumParams(node.getParamsCount());
    currentScope->insertSymbol(newSymbol);

    // Check for the time stmt
    node.getTimeStmt()->accept(*this);

    // Creates a new scope for this event
    std::shared_ptr<Scope> newScope = symtab.enterScope();

    // Inserting parameters in the function scope
    if (node.getParamsCount() > 0) {
        for (int i = 0; i < node.getParamsCount(); i++) {
            if (auto var = dynamic_cast<VariableDecNode *>(node.getParam(i))) {
                Symbol newSymbol(var->getValue(), var, SymbolCategory::PARAMETER, var->getType());
                newScope->insertSymbol(newSymbol);
            }
        }
    }

    node.getCodeBlock()->accept(*this);

    return nullptr;
}

void *SemanticVisitor::visit(ExitNode &node) {
    return nullptr;
};
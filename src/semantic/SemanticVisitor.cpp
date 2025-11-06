#include "SemanticVisitor.h"

namespace llvm {
class Type;
}

void *SemanticVisitor::visit(CodeBlockNode &node) {
    if (node.getStmtCount() < 1) {
        throw std::runtime_error("Empty block of code");
        return nullptr;
    }

    // Visits all the statements inside the block
    for (int i = 0; i < node.getStmtCount(); i++) {
        node.getStmt(i)->accept(*this);
    }

    return nullptr;
}

void *SemanticVisitor::visit(LiteralNode &node) {
    return nullptr;
}

void *SemanticVisitor::visit(BinaryExprNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();
    SupportedTypes LT;
    SupportedTypes RT;

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

    // Getting the type of the variables
    if (auto left = dynamic_cast<VariableRefNode *>(node.getLeft())) {
        std::shared_ptr<Scope> scope = symtab.findScope(left->getValue());
        LT = scope.get()->getSymbol(left->getValue())->getType();
    }

    if (auto right = dynamic_cast<VariableRefNode *>(node.getRight())) {
        std::shared_ptr<Scope> scope = symtab.findScope(right->getValue());
        RT = scope.get()->getSymbol(right->getValue())->getType();
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

    if (currentScope.get()->contains(node.getValue())) {
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
        if (node.getType() != SupportedTypes::TYPE_VOID) {
            throw std::runtime_error("Variable redeclaration error");
        }

        // Check for identifier variable status
        if (currentScope.get()->getSymbol(node.getValue())) {
            Symbol sym = *currentScope.get()->getSymbol(node.getValue());

            if (sym.getCategory() != SymbolCategory::VARIABLE) {
                throw std::runtime_error("Missing declaration for a identifier used in a variable assignment");
            }
        }

        return nullptr;
    }

    /* Type check for variable dec + assign */
    if (auto val = dynamic_cast<BinaryExprNode *>(node.getAssign())) {
        if (val->getType() != node.getType()) {
            throw std::runtime_error("Variable assign with incompatible types, expr: " + typeToString(val->getType()) +
                                     " and variable being assign has: " + typeToString(node.getType()));
        }

        // Inserting the variable in the Symbol Table
        Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
        currentScope->insertSymbol(newSymbol);
    }
    if (auto val = dynamic_cast<LiteralNode *>(node.getAssign())) {
        if (val->getType() != node.getType()) {
            throw std::runtime_error(
                "Variable assign with incompatible types for value: " + typeToString(val->getType()) +
                " ,to a varianble declarated as: " + typeToString(node.getType()));
        }

        // Inserting the variable in the Symbol Table
        Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
        currentScope->insertSymbol(newSymbol);
    }
    if (auto val = dynamic_cast<VariableRefNode *>(node.getAssign())) {
        Symbol sym = *currentScope.get()->getSymbol(val->getValue());

        if (sym.getType() != node.getType()) {
            throw std::runtime_error(
                "Variable assign with incompatible types for value: " + typeToString(sym.getType()) +
                " ,to a varianble declarated as: " + typeToString(node.getType()));
        }

        // Inserting the variable in the Symbol Table
        Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
        currentScope->insertSymbol(newSymbol);
    }

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
    } else {
        throw std::runtime_error("Missing declaration for the identifier: " + node.getValue());
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
    int expectedParams = currentScope.get()->getSymbol(node.getValue())->getNumParams();

    // TODO: CHECK FOR NUM PARAMS IN FUNCTION DEF == PARAMS IN CALL
    if (node.getParamsCount() != expectedParams) {
        throw std::runtime_error("The function " + node.getValue() + " was declared with " +
                                 std::to_string(expectedParams) + " but is being called with " +
                                 std::to_string(node.getParamsCount()));
    }

    // Checking for uses of undefined variable as params
    for (int i = 0; i < node.getParamsCount(); i++) {
        if (auto var = dynamic_cast<VariableRefNode *>(node.getParam(i))) {
            if (!currentScope.get()->contains(var->getValue())) {
                throw std::runtime_error("Missing declaration for a identifier used in a function call: " +
                                         node.getParam(i)->getValue());
            }
        }
    }

    return nullptr;
}

void *SemanticVisitor::visit(ReturnNode &node) {
    node.getStmt()->accept(*this);
    symtab.exitScope();

    return nullptr;
}
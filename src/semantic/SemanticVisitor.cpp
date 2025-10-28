#include "SemanticVisitor.h"

void *SemanticVisitor::visit(LiteralNode &node) {
    return nullptr;
}

void *SemanticVisitor::visit(BinaryExprNode &node) {
    SupportedTypes LT, RT;

    if (auto L = dynamic_cast<LiteralNode *>(node.getLeft())) {
        LT = L->getType();
    }
    /* TODO: CHECK FOR A VARIABLE WITH VALUE
    else if () {
    }
    */

    if (auto R = dynamic_cast<LiteralNode *>(node.getLeft())) {
        RT = R->getType();
    }

    if (LT == RT) {
        node.setType(LT);
    }

    return nullptr;
}

void *SemanticVisitor::visit(CodeBlockNode &node) {
    for (int i = 0; i < node.getStmtCount(); i++) {
        node.getStmt(i)->accept(*this);
    }
    return nullptr;
}

void *SemanticVisitor::visit(VariableDecNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    if (currentScope.get()->contains(node.getValue())) {
        std::cerr << "Variable redeclaration error" << std::endl;
    }

    Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE);

    currentScope->insertSymbol(newSymbol);

    return nullptr;
}

void *SemanticVisitor::visit(VariableAssignNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    // Checks if the variable was already declarated
    if (currentScope.get()->contains(node.getValue())) {
        if (node.getType() != SupportedTypes::TYPE_VOID) {
            std::cerr << "Variable redeclaration error" << std::endl;
        }

        // Check for identifier variable status
        if (currentScope.get()->getSymbol(node.getValue())) {
            Symbol sym = *currentScope.get()->getSymbol(node.getValue());

            if (sym.getCategory() != SymbolCategory::VARIABLE) {
                std::cerr << "Non variable identifier used in a variable assign" << std::endl;
            }
        }

        // TODO: Type check for already declarated variable assign

        Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE);

        currentScope->insertSymbol(newSymbol);
    }

    // Type check for variable dec + assign
    if (auto val = dynamic_cast<BinaryExprNode *>(node.getAssign())) {
        if (val->getValue() != node.getValue()) {
            std::cerr << "Variable assign with incompatible types" << std::endl;
        }
    }

    return nullptr;
}
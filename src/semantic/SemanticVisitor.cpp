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

    Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE);

    currentScope->insertSymbol(newSymbol);

    return nullptr;
}
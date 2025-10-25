#include "SemanticVisitor.h"

void *SemanticVisitor::visit(BinaryExprNode &node) {
    return nullptr;
}

void *SemanticVisitor::visit(CodeBlockNode &node) {
    for (int i = 0; i < node.getStmtCount(); i++) {
        node.getStmt(i)->accept(*this);
    }
    return nullptr;
}

void *SemanticVisitor::visit(VariableDecNode &node) {
    Scope *currentScope = symtab.getCurrentScope();

    Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE);

    currentScope->insertSymbol(newSymbol);

    return nullptr;
}
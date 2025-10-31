#include "SemanticVisitor.h"

namespace llvm {
class Type;
}

void *SemanticVisitor::visit(CodeBlockNode &node) {
    if (node.getStmtCount() < 1) {
        std::cerr << "Empty block of code" << std::endl;
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
        std::cerr << "Diferent types in binary experession with left: " << typeToString(LT)
                  << " and right: " << typeToString(RT) << std::endl;
    }

    return nullptr;
}

void *SemanticVisitor::visit(VariableDecNode &node) {
    std::shared_ptr<Scope> currentScope = symtab.getCurrentScope();

    if (currentScope.get()->contains(node.getValue())) {
        std::cerr << "Variable redeclaration error" << std::endl;
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
            std::cerr << "Variable redeclaration error" << std::endl;
        }

        // Check for identifier variable status
        if (currentScope.get()->getSymbol(node.getValue())) {
            Symbol sym = *currentScope.get()->getSymbol(node.getValue());

            if (sym.getCategory() != SymbolCategory::VARIABLE) {
                std::cerr << "Missing declaration for a identifier used in a variable assign" << std::endl;
            }
        }

        return nullptr;
    }

    /* Type check for variable dec + assign */
    if (auto val = dynamic_cast<BinaryExprNode *>(node.getAssign())) {
        if (val->getType() != node.getType()) {
            std::cerr << "Variable assign with incompatible types, expr: " << typeToString(val->getType())
                      << " and variable being assign has: " << typeToString(node.getType()) << std::endl;
        }

        // Inserting the variable in the Symbol Table
        Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
        currentScope->insertSymbol(newSymbol);
    }
    if (auto val = dynamic_cast<LiteralNode *>(node.getAssign())) {
        if (val->getType() != node.getType()) {
            std::cerr << "Variable assign with incompatible types" << std::endl;
        }

        // Inserting the variable in the Symbol Table
        Symbol newSymbol(node.getValue(), &node, SymbolCategory::VARIABLE, node.getType());
        currentScope->insertSymbol(newSymbol);
    }
    if (auto val = dynamic_cast<VariableRefNode *>(node.getAssign())) {
        Symbol sym = *currentScope.get()->getSymbol(val->getValue());

        if (sym.getType() != node.getType()) {
            std::cerr << "Incompatible types in variable assign" << std::endl;
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
        // Check for this identifier variable status
        if (currentScope.get()->getSymbol(node.getValue())) {
            Symbol sym = *currentScope.get()->getSymbol(node.getValue());

            if (sym.getCategory() != SymbolCategory::VARIABLE) {
                std::cerr << "The symbol in use: " << node.getValue() << " is not a variable" << std::endl;
            }
        }
    } else {
        std::cerr << "Missing declaration for the identifier: " << node.getValue() << std::endl;
    }

    return nullptr;
}

void *SemanticVisitor::visit(ReturnNode &node) {
    return nullptr;
}
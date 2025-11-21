#include "SymbolTable.h"

std::shared_ptr<Scope> SymbolTable::enterScope() {
    auto newScope = std::make_shared<Scope>(nextScopeId++, currentScope->getLevel() + 1, currentScope);
    currentScope = newScope;
    scopes.emplace_back(std::move(newScope));

    return currentScope;
}

void SymbolTable::exitScope() {
    if (auto parent = currentScope->getParent()) {
        currentScope = parent;
    } else {
        std::runtime_error("There is no parent scope");
    }
}

std::shared_ptr<Scope> SymbolTable::getScopeByID(int id) {
    return scopes[id];
}

void SymbolTable::print() const {
    // Prints all the scopes
    for (const auto &scope : scopes) {
        scope->print();
    }
}
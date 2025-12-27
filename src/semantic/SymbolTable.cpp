#include "SymbolTable.h"

std::shared_ptr<Scope> SymbolTable::enterScope(bool block) {
    auto newScope = std::make_shared<Scope>(nextScopeId++, currentScope->getLevel() + 1, currentScope, block);
    currentScope = newScope;
    scopes.emplace_back(std::move(newScope));

    return currentScope;
}

void SymbolTable::exitScope() {
    if (auto parent = currentScope->getParent()) {
        currentScope = parent;
    }

    /*
        In case there is no parent scope, this means the symbol table points to the global scope
        so the currentScope should be kept as the actual scope. A pop in the stack would mean the symbol table
        could be in a unsafe state, pointing to a null scope.
    */
}

std::shared_ptr<Scope> SymbolTable::getScopeByID(int id) {
    if (scopes[id]) {
        return scopes[id];
    }

    throw std::runtime_error("Error: can not access a scope with id: " + std::to_string(id));
}

void SymbolTable::print() const {
    // Prints all the scopes
    for (const auto &scope : scopes) {
        scope->print();
    }
}
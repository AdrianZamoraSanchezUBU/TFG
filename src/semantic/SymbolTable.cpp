#include "SymbolTable.h"

Scope *SymbolTable::enterScope() {
    auto newScope = std::make_unique<Scope>(nextScopeId++, currentScope->getLevel() + 1, currentScope);
    currentScope = newScope.get();
    scopes.emplace_back(std::move(newScope));

    return currentScope;
}

void SymbolTable::exitScope() {
    if (currentScope->getParent())
        currentScope = currentScope->getParent();
}

Scope *SymbolTable::findScope(std::string key) {
    for (int i = static_cast<int>(scopes.size()) - 1; i >= 0; --i) {
        if (scopes[i]->contains(key)) {
            return scopes[i].get();
        }
    }

    return nullptr;
}

bool SymbolTable::reach(std::string element1, std::string element2) {
    Scope *scope1 = findScope(element1);
    Scope *scope2 = findScope(element2);

    if (!scope1 || !scope2)
        return false;

    // scope2 es visible para scope1 si scope2 es padre (o el mismo) de scope1
    Scope *current = scope1;
    while (current != nullptr) {
        if (current == scope2)
            return true;
        current = current->getParent();
    }
    return false;
}

void SymbolTable::print() {
    // Prints all the scopes
    for (const auto &scope : scopes) {
        scope->print();
    }
}
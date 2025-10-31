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
    }
}

std::shared_ptr<Scope> SymbolTable::findScope(const std::string &key) {
    for (int i = static_cast<int>(scopes.size()) - 1; i >= 0; --i) {
        if (scopes[i]->contains(key)) {
            return scopes[i];
        }
    }

    return nullptr;
}

bool SymbolTable::reach(const std::string &element1, const std::string &element2) {
    std::shared_ptr<Scope> scope1 = findScope(element1);
    std::shared_ptr<Scope> scope2 = findScope(element2);

    if (!scope1 || !scope2)
        return false;

    // Scope 2 is visible for Scope 1 if is a higher level Scope or part of it
    std::shared_ptr<Scope> current = scope1;
    while (current != nullptr) {
        if (current == scope2)
            return true;

        auto parent = current->getParent();
        current = parent ? parent : nullptr;
    }
    return false;
}

void SymbolTable::addLlvmVal(const std::string &id, llvm::Value *val) const {
    getCurrentScope().get()->getSymbol(id)->setLlvmValue(val);
}

void SymbolTable::print() const {
    // Prints all the scopes
    for (const auto &scope : scopes) {
        scope->print();
    }
}
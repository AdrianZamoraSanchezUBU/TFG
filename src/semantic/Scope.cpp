#include "Scope.h"

bool Scope::contains(std::string id) {
    // Look for the id in this scopes
    if (symbols.find(id) != symbols.end()) {
        return true;
    }

    // Stops the search if the parent scope is the global scope
    if (parent == nullptr) {
        return false;
    }

    // Look for the id in parent
    return parent->contains(id);
}

bool Scope::insertSymbol(Symbol symbol) {}
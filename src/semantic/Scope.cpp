#include "Scope.h"

std::string Symbol::print() const {
    std::string cat;

    switch (category) {
    case SymbolCategory::CONSTANT:
        cat = "CONSTANT";
        break;
    case SymbolCategory::FUNCTION:
        cat = "FUNCTION";
        break;
    case SymbolCategory::PARAMETER:
        cat = "PARAMETER";
        break;
    case SymbolCategory::VARIABLE:
        cat = "VARIABLE";
        break;
    }

    return cat;
};

bool Scope::contains(std::string id) {
    // Look for the id in this Scope
    if (symbols.find(id) != symbols.end()) {
        return true;
    }

    // Try to get the parent scope
    if (auto p = parent.lock()) {
        // Look for the id in parent
        return p->contains(id);
    }

    // No parent (global scope)
    return false;
}

bool Scope::insertSymbol(Symbol symbol) {
    auto ret = symbols.find(symbol.getID());

    // Inserts the new Symbol if was not included before
    if (ret == symbols.end()) {
        symbols.insert({symbol.getID(), symbol});
        return true;
    }

    return false;
}

void Scope::print() {
    std::cout << "Scope nº " << id << " at level " << level << " with symbols:" << std::endl;

    // Prints the pairs {key,symbol} in the map
    for (const auto &[key, symbol] : symbols) {
        std::cout << "- {Key: " << key << ",(" << symbol.print() << ")}" << std::endl;
    }
}
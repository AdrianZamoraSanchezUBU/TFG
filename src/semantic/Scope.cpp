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

bool Scope::contains(const std::string &id) {
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
};

std::shared_ptr<Scope> Scope::findLocalScope(const std::string &id) {
    if (symbols.find(id) != symbols.end()) {
        return shared_from_this();
    }
    if (auto p = parent.lock()) {
        return p->findLocalScope(id);
    }
    return nullptr;
}

Symbol *Scope::getSymbol(const std::string &id) {
    auto it = symbols.find(id);

    if (it != symbols.end()) {
        return &it->second;
    }

    // Not found in this scope, try parent
    if (auto p = parent.lock()) {
        return p->getSymbol(id);
    }

    std::cerr << "Cant find symbol named: " + id + " in Scope at level: " + std::to_string(level) +
                     " with id: " + std::to_string(this->id)
              << std::endl;

    return nullptr;
};

bool Scope::insertSymbol(Symbol symbol) {
    auto ret = symbols.find(symbol.getID());

    // Inserts the new Symbol if was not included before
    if (ret == symbols.end()) {
        symbols.insert({symbol.getID(), symbol});
        return true;
    }

    std::cerr << "Symbol already present in symbol table" << std::endl;
    return false;
};

void Scope::print() const {
    std::cout << "Scope nº " << id << " at level " << level << " with symbols:" << std::endl;

    // Prints the pairs {key,symbol} in the map
    for (const auto &[key, symbol] : symbols) {
        std::cout << "- {Key: " << key << ",(" << symbol.print() << "," << typeToString(symbol.getType()) << ")}"
                  << std::endl;
    }
};
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
    case SymbolCategory::EVENT:
        cat = "EVENT";
        break;
    }

    return ID + " " + cat + " " + typeToString(type);
};

bool Scope::contains(const std::string &id) {
    // Look for the id in this Scope
    if (symbols.find(id) != symbols.end()) {
        return true;
    }

    // Symbol not present
    return false;
};

Symbol *Scope::getSymbol(const std::string &id) {
    auto it = symbols.find(id);

    if (it != symbols.end()) {
        return &it->second;
    }

    // Not found in this scope, try parent
    if (auto p = parent.lock()) {
        return p->getSymbol(id);
    }

    return nullptr;
};

bool Scope::insertSymbol(Symbol symbol) {
    auto ret = symbols.find(symbol.getID());

    // Inserts the new Symbol if was not included before
    if (ret == symbols.end()) {
        symbols.insert({symbol.getID(), symbol});
        return true;
    }

    spdlog::warn("Symbol already present in symbol table");
    return false;
};

void Scope::print() const {
    spdlog::debug("Scope nº {} at level {} with symbols:", id, level);

    // Prints the pairs {key,symbol} in the map
    for (const auto &[key, symbol] : symbols) {
        spdlog::debug("- Key: {} , {}", key, symbol.print());
    }
};
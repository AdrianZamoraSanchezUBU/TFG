#include "Scope.h"

std::string Symbol::print() const {
    std::string type;

    if (auto *VarDecNode = dynamic_cast<VariableDecNode *>(node)) {
        type = typeToString(VarDecNode->getType());
    }

    return type + " " + this->ID;
};

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
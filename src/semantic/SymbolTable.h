/**
 * @file SymbolTable.h
 * @brief Structure that manages the identifiers and the reach they have between code blocks.
 *
 * @author Adrián Zamora Sánchez
 */
#include "Scope.h"
#include <vector>

/**
 * @brief Manages all declared identifiers and their visibility across nested scopes.
 * @see Scope.h
 */
class SymbolTable {
    std::vector<std::unique_ptr<Scope>> scopes;
    Scope *currentScope = nullptr;
    int nextScopeId = 0;

  public:
    SymbolTable() {
        // Global Scope with level 0
        currentScope = new Scope(nextScopeId++, 0, nullptr);
        scopes.emplace_back(currentScope);
    }

    /**
     * @brief Creates a new Scope and uses it as current Scope.
     */
    Scope *enterScope();

    /**
     * @brief Exits the current Scope, changing the current Scope to its father Scope.
     */
    void exitScope();

    /**
     * @brief Finds the Scope that contains the key.
     */
    Scope *findScope(std::string key);

    /**
     * @brief Returns `true` if the first element can reach the second one, `false`otherwise.
     */
    bool reach(std::string element1, std::string element2);

    /**
     * @brief Returns the current Scope.
     */
    Scope *getCurrentScope() { return currentScope; }

    /**
     * @brief Prints all the Scopes and its Symbols.
     */
    void print();
};

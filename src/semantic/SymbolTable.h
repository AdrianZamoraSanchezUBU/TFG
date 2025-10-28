/**
 * @file SymbolTable.h
 * @brief Structure that manages the identifiers and the reach they have between code blocks.
 *
 * @author Adrián Zamora Sánchez
 */
#pragma once
#include "Scope.h"
#include <vector>

/**
 * @brief Manages all declared identifiers and their visibility across nested scopes.
 * @see Scope.h
 */
class SymbolTable {
    std::vector<std::shared_ptr<Scope>> scopes;
    std::shared_ptr<Scope> currentScope = nullptr;
    int nextScopeId = 0;

  public:
    /**
     * @brief Constructor for SymbolTable.
     */
    SymbolTable() {
        // Global Scope with level 0
        currentScope = std::make_shared<Scope>(nextScopeId++, 0, nullptr);
        scopes.emplace_back(currentScope);
    }

    /**
     * @brief Creates a new Scope and uses it as current Scope.
     */
    std::shared_ptr<Scope> enterScope();

    /**
     * @brief Exits the current Scope, changing the current Scope to its father Scope.
     */
    void exitScope();

    /**
     * @brief Finds the Scope that contains the key.
     */
    std::shared_ptr<Scope> findScope(std::string key);

    /**
     * @brief Returns `true` if the first element can reach the second one, `false`otherwise.
     */
    bool reach(std::string element1, std::string element2);

    /**
     * @brief Returns the current Scope.
     */
    std::shared_ptr<Scope> getCurrentScope() { return currentScope; }

    /**
     * @brief Adds a llvm value to the Symbol associated with the ID.
     */
    void addLlvmVal(std::string, llvm::Value *);

    /**
     * @brief Prints all the Scopes and its Symbols.
     */
    void print();
};

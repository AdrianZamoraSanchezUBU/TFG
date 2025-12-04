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
     *
     * This constructor sets the root scope and the default supported functions of the language.
     */
    explicit SymbolTable() {
        // Global Scope with level 0
        currentScope = std::make_shared<Scope>(nextScopeId++, 0, nullptr);

        // Adding language integrated functions
        Symbol printf("printf", SymbolCategory::FUNCTION, SupportedTypes::TYPE_INT);
        currentScope->insertSymbol(printf);

        Symbol strlen("strlen", SymbolCategory::FUNCTION, SupportedTypes::TYPE_INT);
        currentScope->insertSymbol(strlen);

        Symbol toString("toString", SymbolCategory::FUNCTION, SupportedTypes::TYPE_STRING);
        currentScope->insertSymbol(toString);

        scopes.emplace_back(currentScope);
    }

    /**
     * @brief Creates a new Scope and uses it as current Scope.
     * @return New Scope.
     */
    std::shared_ptr<Scope> enterScope();

    /// Exits the current Scope, changing the current Scope to its father Scope.
    void exitScope();

    /**
     * @brief Gets the scope with the searched id.
     * @param id Identifier of the searched Scope.
     * @see IRGenerator.cpp
     */
    std::shared_ptr<Scope> getScopeByID(int id);

    /**
     * @brief Sets the current scope.
     * @param scope The new current Scope.
     */
    void setCurrentScope(std::shared_ptr<Scope> scope) { currentScope = scope; };

    /**
     * @brief Returns the current Scope.
     * @return The current Scope.
     */
    std::shared_ptr<Scope> getCurrentScope() const { return currentScope; }

    /**
     * @brief Prints all the Scopes and its Symbols.
     */
    void print() const;
};

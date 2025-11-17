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
     */
    std::shared_ptr<Scope> enterScope();

    /**
     * @brief Exits the current Scope, changing the current Scope to its father Scope.
     */
    void exitScope();

    /**
     * @brief Finds the Scope that contains the key.
     */
    std::shared_ptr<Scope> findScope(const std::string &);

    /**
     * @brief Returns `true` if the first element can reach the second one, `false`otherwise.
     */
    bool reach(const std::string &, const std::string &);

    /**
     * @brief Returns the current Scope.
     */
    std::shared_ptr<Scope> getCurrentScope() const { return currentScope; }

    /**
     * @brief Adds a llvm value to the Symbol associated with the ID.
     *
     * This is a direct binding from SymbolTable to Symbol, this method should only
     * be used in a compiler phase posterior to semantic analysis.
     */
    void addLlvmValue(const std::string &, llvm::Value *);

    /**
     * @brief Gets a llvm value from the Symbol associated with the ID.
     *
     * This is a direct binding from SymbolTable to Symbol, this method should only
     * be used in a compiler phase posterior to semantic analysis.
     */
    llvm::Value *getLlvmValue(const std::string &);

    /**
     * @brief Prints all the Scopes and its Symbols.
     */
    void print() const;
};

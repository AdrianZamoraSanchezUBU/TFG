/**
 * @file SymbolTable.h
 * @brief Structure that manages the identifiers and the reach they have between code blocks.
 *
 * @author Adrián Zamora Sánchez
 */
#include "Scope.h"
#include <stack>

/**
 * @brief Manages all declared identifiers and their visibility across nested scopes.
 * @see Scope.h
 */
class SymbolTable {
    std::stack<Scope> scopes;
};

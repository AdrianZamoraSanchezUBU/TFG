/**
 * @file Scope.h
 * @brief This file declares all the classes that represent a Scope and the Symbols it holds.
 *
 * @author Adrián Zamora Sánchez
 */
#include "AST.h"
#include <string>
#include <unordered_map>

namespace llvm {
class Value;
}

/**
 * @brief Possible categories for identifiers.
 */
enum SymbolCategory {
    VARIABLE,  ///< Regular variable
    FUNCTION,  ///< Function
    PARAMETER, ///< Function parameter
    CONSTANT   ///< Constant value (might not be implemented)
};

/**
 * @brief Represents a declared identifier in the program.
 * @see AST.h
 */
class Symbol {
    std::string ID;
    ASTNode *node;
    llvm::Value *llvmVal;
    SymbolCategory category;
};

/**
 * @brief Represents a single scope/block in the program.
 * @see SymbolTable.h
 */
class Scope {
    std::unordered_map<std::string, Symbol> symbols;
};

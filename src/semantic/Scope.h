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

  public:
    /**
     * @brief Constructor for Symbol.
     */
    Symbol(std::string id, ASTNode *astnode, llvm::Value *val, SymbolCategory cat)
        : ID(id), node(astnode), llvmVal(val), category(cat) {}

    Symbol(std::string id, ASTNode *astnode, SymbolCategory cat) : ID(id), node(astnode), category(cat) {}

    /**
     * @brief Getter for ID;
     */
    std::string getID() { return ID; }

    /**
     * @brief Prints the Symbol data.
     */
    std::string print() const;
};

/**
 * @brief Represents a single scope/block in the program.
 * @see SymbolTable.h
 */
class Scope {
    int id;
    int level;
    Scope *parent;
    std::unordered_map<std::string, Symbol> symbols;

  public:
    /**
     * @brief Constructor for Scope
     * @param id unique identifier for this Scope.
     * @param level level of visibility of this Scope.
     * @param parent lower level parent Scope.
     */
    Scope(int id, int level, Scope *parent) : id(id), level(level), parent(parent) {}

    /**
     * @brief Returns `true`if this scope contains this id, `false`otherwise.
     */
    bool contains(std::string id);

    /**
     * @brief Returns the parent Scope.
     */
    Scope *getParent() { return parent; }

    /**
     * @brief Inserts a new Symbol in this Scope.
     * @see Symbol
     */
    bool insertSymbol(Symbol);

    /**
     * @brief Returns the level of this Scope.
     */
    int getLevel() { return level; }

    /**
     * @brief Prints Scope data.
     */
    void print();
};

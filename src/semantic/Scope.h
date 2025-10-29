/**
 * @file Scope.h
 * @brief This file declares all the classes that represent a Scope and the Symbols it holds.
 *
 * @author Adrián Zamora Sánchez
 */
#pragma once
#include "AST.h"
#include <string>
#include <unordered_map>

namespace llvm {
class Value;
class Type;
} // namespace llvm

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
    SupportedTypes type = SupportedTypes::TYPE_VOID;
    SymbolCategory category;

  public:
    /**
     * @brief Constructor for Symbol.
     */
    Symbol(std::string id, ASTNode *astnode, llvm::Value *val, SymbolCategory cat, SupportedTypes ty)
        : ID(id), node(astnode), llvmVal(val), category(cat), type(ty) {}

    Symbol(std::string id, ASTNode *astnode, SymbolCategory cat, SupportedTypes ty)
        : ID(id), node(astnode), category(cat), type(ty) {}

    /**
     * @brief Getter for ID.
     */
    std::string getID() { return ID; }

    /**
     * @brief Getter for category.
     */
    SymbolCategory getCategory() { return category; }

    /**
     * @brief Getter for type.
     */
    SupportedTypes getSupportedType() { return type; }

    /**
     * @brief Setter for type.
     */
    void setType(SupportedTypes ty) { type = ty; }

    /**
     * @brief Setter for llvmVal.
     */
    void setLlvmValue(llvm::Value *val) { llvmVal = val; }

    /**
     * @brief Setter for llvmVal.
     */
    llvm::Value *getLlvmValue() { return llvmVal; }

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
    std::weak_ptr<Scope> parent;
    std::unordered_map<std::string, Symbol> symbols;

  public:
    /**
     * @brief Constructor for Scope
     * @param id unique identifier for this Scope.
     * @param level level of visibility of this Scope.
     * @param parent lower level parent Scope.
     */
    Scope(int id, int level, std::shared_ptr<Scope> parent) : id(id), level(level), parent(parent) {}

    /**
     * @brief Returns `true`if this scope contains this id, `false`otherwise.
     */
    bool contains(std::string id);

    /**
     * @brief Returns the parent Scope.
     */
    std::shared_ptr<Scope> getParent() {
        return parent.lock();
        ;
    }

    /**
     * @brief Inserts a new Symbol in this Scope.
     * @see Symbol
     */
    bool insertSymbol(Symbol);

    /**
     * @brief Getter for Symbol
     */
    Symbol *getSymbol(const std::string &id) {
        auto it = symbols.find(id);
        if (it != symbols.end())
            return &it->second;

        return nullptr;
    }

    /**
     * @brief Returns the level of this Scope.
     */
    int getLevel() { return level; }

    /**
     * @brief Prints Scope data.
     */
    void print();
};

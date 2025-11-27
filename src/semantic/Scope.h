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
    POINTER,   ///< Pointer
    CONSTANT   ///< Constant value
};

/**
 * @brief Represents a declared identifier in the program.
 * @see AST.h
 */
class Symbol {
    std::string ID;
    ASTNode *node;
    llvm::Value *llvmVal;
    Type type;
    SymbolCategory category;
    int numParams;

  public:
    /**
     * @brief Constructor for Symbol.
     */
    explicit Symbol(std::string id, ASTNode *astnode, llvm::Value *val, SymbolCategory cat, Type ty)
        : ID(id), node(astnode), llvmVal(val), category(cat), type(ty) {}

    explicit Symbol(std::string id, ASTNode *astnode, SymbolCategory cat, Type ty)
        : ID(id), node(astnode), category(cat), type(ty) {}

    explicit Symbol(std::string id, SymbolCategory cat, Type ty) : ID(id), node(nullptr), category(cat), type(ty) {}

    /**
     * @brief Getter for ID.
     */
    std::string getID() const { return ID; }

    /**
     * @brief Getter for category.
     */
    SymbolCategory getCategory() const { return category; }

    /**
     * @brief Returns true if this symbol represents a ptr, false otherwise.
     */
    bool isPtr() const;

    /**
     * @brief Getter for the type or pointed type.
     */
    SupportedTypes getType() const;

    /**
     * @brief Setter for type.
     */
    void setType(Type ty) { type = ty; }

    /**
     * @brief Getter for number of params.
     */
    int getNumParams() const { return numParams; }

    /**
     * @brief Setter for number of params.
     */
    void setNumParams(int params) { numParams = params; }

    /**
     * @brief Setter for llvmVal.
     */
    void setLlvmValue(llvm::Value *val) { llvmVal = val; }

    /**
     * @brief Setter for llvmVal.
     */
    llvm::Value *getLlvmValue() const { return llvmVal; }

    /**
     * @brief Prints the Symbol data.
     */
    std::string print() const;
};

/**
 * @brief Represents a single scope/block in the program.
 * @see SymbolTable.h
 */
class Scope : public std::enable_shared_from_this<Scope> {
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
    explicit Scope(int id, int level, std::shared_ptr<Scope> parent) : id(id), level(level), parent(parent) {}

    /**
     * @brief Returns `true` if this scope or his parents contains this id, `false`otherwise.
     */
    bool contains(const std::string &);

    /**
     * @brief Returns the parent Scope.
     */
    std::shared_ptr<Scope> getParent() const { return parent.lock(); }

    /**
     * @brief Inserts a new Symbol in this Scope.
     * @see Symbol
     */
    bool insertSymbol(Symbol);

    /**
     * @brief Getter for Symbol
     */
    Symbol *getSymbol(const std::string &);

    /**
     * @brief Returns the level of this Scope.
     */
    int getLevel() const { return level; }

    /**
     * @brief Returns the id of this Scope.
     */
    int getID() const { return id; }

    /**
     * @brief Prints Scope data.
     */
    void print() const;
};

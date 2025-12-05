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

/// Valid categories for the identifiers.
enum SymbolCategory {
    VARIABLE,  ///< Regular variable
    FUNCTION,  ///< Function
    EVENT,     ///< Event
    PARAMETER, ///< Function parameter
    CONSTANT   ///< Constant value
};

/**
 * @brief Represents a declared identifier in the program.
 * @see AST.h
 * @see Type.h
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
     * @return String with this symbol's name identifier.
     */
    std::string getID() const { return ID; }

    /**
     * @brief Getter for category.
     * @return Category of this symbol.
     */
    SymbolCategory getCategory() const { return category; }

    /**
     * @brief Returns a true if the symbol is a pointer.
     * @return `true` if this symbol represents a ptr, `false` otherwise.
     */
    bool isPtr() const {
        if (type.base) {
            return true;
        }

        return false;
    };

    /// Getter for the type or pointed type.
    SupportedTypes getType() const { return type.getSupportedType(); };

    /**
     * @brief Setter for type.
     * @param ty New Type for the Symbol.
     */
    void setType(Type ty) { type = ty; }

    /**
     * @brief Getter for number of params.
     * @return Amount of parameters.
     */
    int getNumParams() const { return numParams; }

    /**
     * @brief Setter for number of params.
     * @param params New number of parameters of this symbol.
     */
    void setNumParams(int params) {
        if (category != SymbolCategory::FUNCTION && category != SymbolCategory::EVENT) {
            throw std::runtime_error("Unexpected parameters in a non function symbol");
        }

        numParams = params;
    }

    /**
     * @brief Setter for llvmVal.
     * @param val New LLVM Value for this symbol.
     */
    void setLlvmValue(llvm::Value *val) { llvmVal = val; }

    /**
     * @brief Getter for llvmVal.
     * @return LLVM Value of this symbol.
     */
    llvm::Value *getLlvmValue() const { return llvmVal; }

    /**
     * @brief Generates a string with the Symbol data.
     * @return Symbol data string.
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
     * @brief Return true if the Symbol associated with the id is contained in the table.
     * @param id identifier of the searched Symbol.
     * @return Returns `true` if this scope or his parents contains this id, `false`otherwise
     */
    bool contains(const std::string &id);

    /**
     * @brief Returns the parent Scope.
     * @return parent Scope of this Scope.
     */
    std::shared_ptr<Scope> getParent() const { return parent.lock(); }

    /**
     * @brief Inserts a new Symbol in this Scope.
     * @param sym Symbol that will be inserted.
     * @return true if the symbol could be inserted, false if the symbol was already in the scope.
     *
     * @see Symbol
     */
    bool insertSymbol(Symbol sym);

    /**
     * @brief Getter for Symbol
     * @param id identifier of the searched Symbol.
     * @return Symbol if found.
     */
    Symbol *getSymbol(const std::string &id);

    /**
     * @brief Returns the level of this Scope.
     * @return Level of this Scope
     */
    int getLevel() const { return level; }

    /**
     * @brief Returns the id of this Scope.
     * @return identifier of this Scope.
     */
    int getID() const { return id; }

    /**
     * @brief Prints Scope data.
     */
    void print() const;
};

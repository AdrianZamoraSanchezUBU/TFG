/**
 * @file Type.h
 * @brief Types definition for this language and compiler.
 *
 * @author Adrián Zamora Sánchez
 */

#pragma once
#include <iostream>
#include <string>

/// Supported types in the language.
enum SupportedTypes { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL, TYPE_VOID, TYPE_TIME, TYPE_PTR };

/// Type structure for the basic types or pointed types.
struct Type {
    SupportedTypes type; ///< Type
    Type *base;          ///< Pointing to a type

  public:
    /// Empty type constructor
    Type() : type(SupportedTypes::TYPE_VOID), base(nullptr) {}

    /// Basic type constructor
    Type(SupportedTypes ty) : type(ty), base(nullptr) {}

    /// Pointer type constructor
    Type(Type *b) : type(SupportedTypes::TYPE_PTR), base(b) {}

    /// Getter for the type or pointed type.
    SupportedTypes getSupportedType() const {
        if (base == nullptr) {
            return type;
        }

        return base->getSupportedType();
    };

    /// Equal operator
    bool operator==(const Type &other) const { return type == other.type && base == other.base; }

    /// Not equal operator
    bool operator!=(const Type &other) const { return !(*this == other); }
};

/**
 * @brief Returns a string with a type representation.
 * @param type Type object.
 * @return String representation of the type.
 */
inline std::string supportedTypeToString(SupportedTypes type) {
    switch (type) {
    case SupportedTypes::TYPE_INT:
        return "int";
    case SupportedTypes::TYPE_FLOAT:
        return "float";
    case SupportedTypes::TYPE_CHAR:
        return "char";
    case SupportedTypes::TYPE_STRING:
        return "string";
    case SupportedTypes::TYPE_BOOL:
        return "bool";
    case SupportedTypes::TYPE_VOID:
        return "void";
    case SupportedTypes::TYPE_TIME:
        return "time";
    default:
        return "UnknownType";
    }
}

/**
 * @brief Returns a string with a type representation.
 * @param type Type object.
 * @return String representation of the type.
 */
inline std::string typeToString(Type type) {
    // In case of pointer returns a "ptr->" string before the type
    if (type.base != nullptr) {
        return "ptr->" + typeToString(*type.base);
    }

    // Returns the supported type in string format
    return supportedTypeToString(type.type);
}
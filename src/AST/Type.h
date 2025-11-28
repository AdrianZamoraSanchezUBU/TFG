#pragma once
#include <iostream>
#include <string>

/**
 * @brief Supported types in the AST
 */
enum SupportedTypes { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL, TYPE_VOID, TYPE_TIME, TYPE_PTR };

/**
 * @brief Type structure for pointed types support.
 */
struct Type {
    SupportedTypes type;
    Type *base;

  public:
    /**
     * @brief Default constructors
     */
    Type() : type(SupportedTypes::TYPE_VOID), base(nullptr) {}
    Type(SupportedTypes k, Type *b = nullptr) : type(k), base(b) {}

    /**
     * @brief Getter for the type or pointed type.
     */
    SupportedTypes getSupportedType() const {
        if (base == nullptr) {
            return type;
        }

        return base->getSupportedType();
    };

    ///@brief Equal operator
    bool operator==(const Type &other) const { return type == other.type && base == other.base; }

    ///@brief Not equal operator
    bool operator!=(const Type &other) const { return !(*this == other); }
};

/**
 * @brief Returns a string with a type representation.
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
#pragma once
#include <string>

/**
 * @brief Supported types in the AST
 */
enum SupportedTypes { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL };

inline std::string typeToString(SupportedTypes type) {
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
    default:
        return "UnknownType";
    }
}
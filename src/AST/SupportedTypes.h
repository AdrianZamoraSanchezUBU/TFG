#pragma once
#include <iostream>
#include <string>
/**
 * @brief Supported types in the AST
 */
enum SupportedTypes { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL, TYPE_VOID, TYPE_TIME };

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
    case SupportedTypes::TYPE_VOID:
        return "void";
    case SupportedTypes::TYPE_TIME:
        return "time";
    default:
        return "UnknownType";
    }
}
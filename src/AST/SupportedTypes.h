#pragma once
#include <iostream>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <string>
/**
 * @brief Supported types in the AST
 */
enum SupportedTypes { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL, TYPE_VOID };

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
    default:
        return "UnknownType";
    }
}

inline SupportedTypes llvmTypeToSupportedType(llvm::Type *type) {
    if (type->isIntegerTy(32))
        return SupportedTypes::TYPE_INT;

    if (type->isFloatTy())
        return SupportedTypes::TYPE_FLOAT;

    if (type->isIntegerTy(8))
        return SupportedTypes::TYPE_CHAR;

    if (type->isIntegerTy(1))
        return SupportedTypes::TYPE_BOOL;

    if (type->isVoidTy())
        return SupportedTypes::TYPE_VOID;

    if (type->isPointerTy() && type->isIntegerTy())
        return SupportedTypes::TYPE_STRING;

    return SupportedTypes::TYPE_VOID;
}

inline llvm::Type *supportedTypeToLlvmType(SupportedTypes type, llvm::LLVMContext &context) {
    switch (type) {
    case SupportedTypes::TYPE_INT:
        return llvm::Type::getInt32Ty(context);
    case SupportedTypes::TYPE_FLOAT:
        return llvm::Type::getFloatTy(context);
    case SupportedTypes::TYPE_CHAR:
        return llvm::Type::getInt8Ty(context);
    case SupportedTypes::TYPE_BOOL:
        return llvm::Type::getInt1Ty(context);
    case SupportedTypes::TYPE_STRING:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0);
    case SupportedTypes::TYPE_VOID:
        return llvm::Type::getVoidTy(context);
    default:
        return nullptr;
    }
}
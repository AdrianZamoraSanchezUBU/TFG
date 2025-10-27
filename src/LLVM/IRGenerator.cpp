#include "IRGenerator.h"
#include <string.h>

llvm::Value *IRGenerator::visit(CodeBlockNode &node) {
    return nullptr;
}

llvm::Value *IRGenerator::visit(LiteralNode &node) {
    std::variant<int, float, char, std::string, bool> value = node.getVariantValue();

    switch (node.getType()) {
    case SupportedTypes::TYPE_FLOAT: {
        float v = std::get<float>(value);

        // FLOAT
        return llvm::ConstantFP::get(ctx.IRContext, llvm::APFloat(v));
    }
    case SupportedTypes::TYPE_INT: {
        int v = std::get<int>(value);

        // INT: signed 32 bits integer
        return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(32, v, true));
    }
    case SupportedTypes::TYPE_CHAR: {
        char v = std::get<char>(value);

        // CHAR: unsigned 8 bits integer
        return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(8, v, false));
    }
    case SupportedTypes::TYPE_STRING: {
        // Generates a UndefValue as placeholder for future constant folding
        return llvm::UndefValue::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0));
    }
    case SupportedTypes::TYPE_BOOL: {
        bool v = std::get<bool>(value);

        // BOOL: unsigned 1 bit integer
        return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(1, v ? 1 : 0, false));
    }
    default:
        // Checks for invalid types in literal nodes
        llvm::errs() << "Unsupported type in literal node: " << node.getValue() << "\n";
        return nullptr;
    }
}

llvm::Value *IRGenerator::visit(BinaryExprNode &node) {
    // Left and Right child nodes visit
    llvm::Value *L = node.getLeft()->accept(*this);
    llvm::Value *R = node.getRight()->accept(*this);

    SupportedTypes LT, RT;
    SupportedTypes operationType = node.getType();

    if (auto left = dynamic_cast<LiteralNode *>(node.getLeft())) {
        LT = left->getType();
    }
    if (auto right = dynamic_cast<LiteralNode *>(node.getRight())) {
        RT = right->getType();
    }

    // Check for correctness in child nodes visits
    if (!L || !R) {
        llvm::errs() << "Null operand in binary expression.\n";
        return nullptr;
    }

    /*
    Type domination: float over int

    If one side of the operation is float and the other is int,
    the int will be cast into a float and then proceed with
    the operation.
    */
    if (LT == SupportedTypes::TYPE_INT && RT == SupportedTypes::TYPE_FLOAT) {
        L = ctx.IRBuilder.CreateSIToFP(L, R->getType(), "inttofloat");
    } else if (LT == SupportedTypes::TYPE_FLOAT && RT == SupportedTypes::TYPE_INT) {
        R = ctx.IRBuilder.CreateSIToFP(R, L->getType(), "inttofloat");
    }

    std::string op = node.getValue();

    /* Numeric operations */
    // FLOAT
    if (operationType == SupportedTypes::TYPE_FLOAT) {
        if (op == "+")
            return ctx.IRBuilder.CreateFAdd(L, R, "addtmp");
        if (op == "-")
            return ctx.IRBuilder.CreateFSub(L, R, "subtmp");
        if (op == "*")
            return ctx.IRBuilder.CreateFMul(L, R, "multmp");
        if (op == "/")
            return ctx.IRBuilder.CreateFDiv(L, R, "divtmp");
        if (op == "==")
            return ctx.IRBuilder.CreateFCmpOEQ(L, R, "eqtmp");
        if (op == "!=")
            return ctx.IRBuilder.CreateFCmpONE(L, R, "netmp");
        if (op == "<")
            return ctx.IRBuilder.CreateFCmpOLT(L, R, "lttmp");
        if (op == ">")
            return ctx.IRBuilder.CreateFCmpOGT(L, R, "gttmp");
    }

    // INT
    if (operationType == SupportedTypes::TYPE_INT || operationType == SupportedTypes::TYPE_CHAR ||
        operationType == SupportedTypes::TYPE_BOOL) {
        if (op == "+")
            return ctx.IRBuilder.CreateAdd(L, R, "addtmp");
        if (op == "-")
            return ctx.IRBuilder.CreateSub(L, R, "subtmp");
        if (op == "*")
            return ctx.IRBuilder.CreateMul(L, R, "multmp");
        if (op == "/")
            return ctx.IRBuilder.CreateSDiv(L, R, "divtmp");
        if (op == "==")
            return ctx.IRBuilder.CreateICmpEQ(L, R, "eqtmp");
        if (op == "!=")
            return ctx.IRBuilder.CreateICmpNE(L, R, "netmp");
        if (op == "<")
            return ctx.IRBuilder.CreateICmpSLT(L, R, "lttmp");
        if (op == ">")
            return ctx.IRBuilder.CreateICmpSGT(L, R, "gttmp");
    }

    // STRING value for constant folding (only '==' and '!=' comparations)
    if (operationType == SupportedTypes::TYPE_STRING) {
        // Check for valid string operation
        if (op == "==" || op == "!=") {
            bool sameStrings = (node.getLeft()->getValue() == node.getRight()->getValue());
            auto *boolVal = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx.IRContext), sameStrings);

            // If the operator is '!=', invert the result
            if (op == "!=") {
                boolVal = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx.IRContext), !sameStrings);
            }

            return boolVal;
        }
    }

    // Checks for invalid operations in the expression
    llvm::errs() << "Unsupported binary operation: " << op << " on given types.\n";
    return nullptr;
}

llvm::Value *IRGenerator::visit(VariableDecNode &node) {
    llvm::Type *varType = nullptr;

    // Type dispatch from Supported Type to LLVM::Type
    switch (node.getType()) {
    case SupportedTypes::TYPE_INT:
        varType = llvm::Type::getInt32Ty(ctx.IRContext);
        break;
    case SupportedTypes::TYPE_FLOAT:
        varType = llvm::Type::getInt32Ty(ctx.IRContext);
        break;
    case SupportedTypes::TYPE_CHAR:
        varType = llvm::Type::getInt32Ty(ctx.IRContext);
        break;
    case SupportedTypes::TYPE_STRING:
        varType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0);
        break;
    case SupportedTypes::TYPE_BOOL:
        varType = llvm::Type::getInt32Ty(ctx.IRContext);
        break;
    }

    // Gets the current function
    llvm::Function *currentFunction = ctx.getCurrentFunction();

    // Creates a temporal builder that points to the begin of the current basic block
    llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());

    // Allocating memory for the variable
    llvm::AllocaInst *allocaInst = tmpBuilder.CreateAlloca(varType, nullptr, node.getValue());

    // Registers this new allocation associated with a Symbol in the SymbolTable
    symtab.addLlvmVal(node.getValue(), allocaInst);

    return allocaInst;
}
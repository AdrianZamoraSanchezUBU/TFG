#include "IRGenerator.h"

IRGenerator::IRGenerator() {
    // Program main function set up
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx.IRContext), false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", ctx.IRModule.get());

    // Basic block set up TODO: include in ASTBuilder and ASTgenerator "program
    // block"
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(ctx.IRContext, "entry", F);
    ctx.IRBuilder.SetInsertPoint(BB);
}

llvm::Value *IRGenerator::visit(LiteralNode &node) {
    std::variant<int, float, char, std::string, bool> value = node.getVariantValue();

    if (std::holds_alternative<int>(value)) {
        int v = std::get<int>(value);

        // INT: signed 32 bits integer
        return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(32, v, true));
    }

    if (std::holds_alternative<float>(value)) {
        float v = std::get<float>(value);

        // FLOAT
        return llvm::ConstantFP::get(ctx.IRContext, llvm::APFloat(v));
    }

    if (std::holds_alternative<char>(value)) {
        char v = std::get<char>(value);

        // CHAR: unsigned 8 bits integer
        return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(8, v, false));
    }

    if (std::holds_alternative<std::string>(value)) {
        std::string v = std::get<std::string>(value);

        // STRING: pointer (*i8) to constant null-terminated IR format string
        return ctx.IRBuilder.CreateGlobalStringPtr(v, ".str");
    }

    if (std::holds_alternative<bool>(value)) {
        bool v = std::get<bool>(value);

        // BOOL: unsigned 1 bit integer
        return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(1, v ? 1 : 0, false));
    }
}

llvm::Value *IRGenerator::visit(BinaryExprNode &node) {
    // left and right LLVM Values
    llvm::Value *L = node.getLeft()->accept(*this);
    llvm::Value *R = node.getRight()->accept(*this);

    // left and right LLVM Types
    llvm::Type *LT = L->getType();
    llvm::Type *RT = R->getType();

    /*
    Type domination: float over int

    If one side of the operation is float and the other is int,
    the int will be cast into a float and then proceed with
    the operation.
    */
    if (LT->isIntegerTy() && RT->isFloatingPointTy()) {
        L = ctx.IRBuilder.CreateSIToFP(L, RT, "inttofloat");
    } else if (LT->isFloatingPointTy() && RT->isIntegerTy()) {
        R = ctx.IRBuilder.CreateSIToFP(R, LT, "inttofloat");
    }

    std::string op = node.getValue();

    /* Arithmetic operations */
    // FLOAT
    if (LT->isFloatingPointTy()) {
        if (op == "+")
            return ctx.IRBuilder.CreateFAdd(L, R, "addtmp");
        if (op == "-")
            return ctx.IRBuilder.CreateFSub(L, R, "subtmp");
        if (op == "*")
            return ctx.IRBuilder.CreateFMul(L, R, "multmp");
        if (op == "/")
            return ctx.IRBuilder.CreateFDiv(L, R, "divtmp");
    }
    // INT
    else if (LT->isIntegerTy()) {
        if (op == "+")
            return ctx.IRBuilder.CreateAdd(L, R, "addtmp");
        if (op == "-")
            return ctx.IRBuilder.CreateSub(L, R, "subtmp");
        if (op == "*")
            return ctx.IRBuilder.CreateMul(L, R, "multmp");
        if (op == "/")
            return ctx.IRBuilder.CreateSDiv(L, R, "divtmp");
    }

    /* Logical operations */
    // FLOAT
    if (LT->isFloatingPointTy()) {
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
    else if (LT->isIntegerTy()) {
        if (op == "==")
            return ctx.IRBuilder.CreateICmpEQ(L, R, "eqtmp");
        if (op == "!=")
            return ctx.IRBuilder.CreateICmpNE(L, R, "netmp");
        if (op == "<")
            return ctx.IRBuilder.CreateICmpSLT(L, R, "lttmp");
        if (op == ">")
            return ctx.IRBuilder.CreateICmpSGT(L, R, "gttmp");
    }

    // STRING
    if (auto *ptrTyL = llvm::dyn_cast<llvm::PointerType>(L->getType())) {
        // C context
        llvm::LLVMContext &C = ctx.IRModule->getContext();
        llvm::Type *i8Ty = llvm::Type::getInt8Ty(C);
        llvm::Type *i8PtrTy = llvm::PointerType::getUnqual(i8Ty);

        // Function strcmp definition
        llvm::FunctionType *strcmpType = llvm::FunctionType::get(llvm::Type::getInt32Ty(C), // Return type
                                                                 {i8PtrTy, i8PtrTy},        // Params type
                                                                 false);

        // Getting the strcmp function
        llvm::FunctionCallee strcmpFunc = ctx.IRModule->getOrInsertFunction("strcmp", strcmpType);

        // Calling strcmp with L and R
        llvm::Value *strcmpResult = ctx.IRBuilder.CreateCall(strcmpFunc, {L, R});

        // Creates a boolean comparation for the result
        llvm::Value *isEqual =
            ctx.IRBuilder.CreateICmpEQ(strcmpResult, llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 0));

        // Returns 0 if the strings are not equal and 1 if they are equal
        return ctx.IRBuilder.CreateSelect(isEqual, llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 1),
                                          llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 0));
    }
}
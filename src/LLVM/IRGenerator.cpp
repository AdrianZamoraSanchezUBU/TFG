#include "IRGenerator.h"
#include <llvm/IR/Verifier.h>
#include <string.h>

llvm::Value *IRGenerator::visit(CodeBlockNode &node) {
    for (int i = 0; i < node.getStmtCount(); i++) {
        node.getStmt(i)->accept(*this);
    }

    return nullptr;
}

llvm::Type *IRGenerator::getLlvmType(SupportedTypes type) {
    switch (type) {
    case SupportedTypes::TYPE_INT:
        return llvm::Type::getInt32Ty(ctx.IRContext);
    case SupportedTypes::TYPE_FLOAT:
        return llvm::Type::getInt32Ty(ctx.IRContext);
    case SupportedTypes::TYPE_CHAR:
        return llvm::Type::getInt32Ty(ctx.IRContext);
    case SupportedTypes::TYPE_STRING:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0);
    case SupportedTypes::TYPE_BOOL:
        return llvm::Type::getInt32Ty(ctx.IRContext);
    default:
        return llvm::Type::getVoidTy(ctx.IRContext);
    }
};

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
        // Throws error for invalid types in literal nodes
        throw std::runtime_error("Unsupported type in literal node: " + node.getValue());
    }
}

llvm::Value *IRGenerator::visit(BinaryExprNode &node) {
    // Left and Right child nodes visit
    llvm::Value *L = node.getLeft()->accept(*this);
    llvm::Value *R = node.getRight()->accept(*this);
    SupportedTypes operationType = node.getType();

    // Check for correctness in child nodes visits
    if (!L || !R) {
        llvm::errs() << "Null operand in binary expression.\n";
        return nullptr;
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

    // Throws error
    throw std::runtime_error("Unsupported binary operation: " + op + " on " + typeToString(operationType));
}

llvm::Value *IRGenerator::visit(VariableDecNode &node) {
    llvm::Type *varType = getLlvmType(node.getType());

    // Gets the current function
    llvm::BasicBlock *currentFunction = ctx.blockStack.back();

    // Creates a temporal builder that points to the begin of the current basic block
    llvm::IRBuilder<> tmpBuilder(currentFunction, currentFunction->begin());

    // Allocating memory for the variable
    llvm::AllocaInst *allocaInst = tmpBuilder.CreateAlloca(varType, nullptr, node.getValue() + "_ptr");

    // Registers this new allocation associated with the Symbol in the SymbolTable
    symtab.addLlvmValue(node.getValue(), allocaInst);

    return allocaInst;
}

llvm::Value *IRGenerator::visit(VariableAssignNode &node) {
    llvm::Value *assignVal;

    // Visits the type of assignment
    if (auto expr = dynamic_cast<BinaryExprNode *>(node.getAssign())) {
        assignVal = visit(*expr);
    } else if (auto lit = dynamic_cast<LiteralNode *>(node.getAssign())) {
        assignVal = visit(*lit);
    } else if (auto var = dynamic_cast<VariableRefNode *>(node.getAssign())) {
        assignVal = visit(*var);
    } else {
        throw std::runtime_error("Not a valid assigment for: " + node.getValue());
    }

    // (DECLARATION + ASSIGNMENT)
    if (node.getType() != SupportedTypes::TYPE_VOID) {
        // Type dispatch from Supported Type to LLVM::Type
        llvm::Type *varType = getLlvmType(node.getType());

        // Gets the current function
        llvm::BasicBlock *currentFunction = ctx.blockStack.back();

        // Creates a temporal builder that points to the begin of the current basic block
        llvm::IRBuilder<> tmpBuilder(currentFunction, currentFunction->begin());

        // Allocating memory for the variable
        llvm::AllocaInst *allocaInst = tmpBuilder.CreateAlloca(varType, nullptr, node.getValue() + "_ptr");

        // Registers this new allocation associated with the Symbol in the SymbolTable
        symtab.addLlvmValue(node.getValue(), allocaInst);

        // Getting the memory address where the value is stored
        llvm::Value *alloc = symtab.getLlvmValue(node.getValue());
        ctx.IRBuilder.CreateStore(assignVal, alloc);

        return alloc;
    }

    // Gets the memory address and stores the value (ONLY ASSIGNMENT)
    llvm::Value *alloc = symtab.getLlvmValue(node.getValue());
    ctx.IRBuilder.CreateStore(assignVal, alloc);

    return alloc;
}

llvm::Value *IRGenerator::visit(VariableRefNode &node) {
    // Finding the ptr of the variable
    std::shared_ptr<Scope> scope = symtab.findScope(node.getValue());
    if (!scope) {
        throw std::runtime_error("There is no scope in the symbol table with the symbol: " + node.getValue());
    }

    auto symbol = scope->getSymbol(node.getValue());
    if (!symbol) {
        throw std::runtime_error("Symbol not found in scope: " + node.getValue());
    }

    llvm::Value *alloc = symtab.getLlvmValue(node.getValue());
    if (!alloc) {
        throw std::runtime_error("There is no value associated to the symbol: " + node.getValue());
    }

    // Returning a direct value
    if (llvm::isa<llvm::Argument>(alloc) || llvm::isa<llvm::Constant>(alloc)) {
        return alloc;
    }

    // Loading the allocated type
    llvm::Type *type;
    if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(alloc)) {
        llvm::Type *type = alloca->getAllocatedType();
        return ctx.IRBuilder.CreateLoad(type, alloca, node.getValue() + "_val");
    }

    throw std::runtime_error("The symbol: " + node.getValue() + " does not have a valid allocation");
}

llvm::Value *IRGenerator::visit(FunctionDefNode &node) {
    // Getting the param definition (only types)
    std::vector<llvm::Type *> paramTypes;
    for (int i = 0; i < node.getParamsCount(); i++) {
        if (auto var = dynamic_cast<VariableDecNode *>(node.getParam(i))) {
            paramTypes.push_back(getLlvmType(var->getType()));
        }
    }

    // Function generation
    llvm::Type *returnType = getLlvmType(node.getType());
    llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    llvm::Function *function = ctx.IRModule->getFunction(node.getValue());

    if (!function) {
        function =
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.getValue(), ctx.IRModule.get());
    }

    // Basic block generation and stack push
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(ctx.IRContext, "entry", function);
    ctx.IRBuilder.SetInsertPoint(entry);
    ctx.pushFunction(entry);

    // Setting all the params as arguments in their symbol in the symbol table
    unsigned idx = 0;
    for (auto &arg : function->args()) {
        auto *varNode = dynamic_cast<VariableDecNode *>(node.getParam(idx++));

        // Sets the value of a arg for direct value access
        std::string name = varNode ? varNode->getValue() : "arg" + std::to_string(idx);
        arg.setName(name);

        symtab.addLlvmValue(name, &arg);
    }

    llvm::verifyFunction(*function);

    // IR generation for all the function statements
    node.getCodeBlock()->accept(*this);

    return function;
};

llvm::Value *IRGenerator::visit(FunctionDecNode &node) {
    // Getting the param definition (only types)
    std::vector<llvm::Type *> paramTypes;
    for (auto param : node.getParams()) {
        paramTypes.push_back(getLlvmType(param));
    }

    // Function generation
    llvm::Type *returnType = getLlvmType(node.getType());
    llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);

    llvm::Function *function =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.getValue(), ctx.IRModule.get());

    return function;
};

llvm::Value *IRGenerator::visit(FunctionCallNode &node) {
    // Function caller
    llvm::Function *callee = ctx.IRModule->getFunction(node.getValue());
    if (!callee) {
        throw std::runtime_error("Undefined function: " + node.getValue());
    }

    // Getting the arguments values
    std::vector<llvm::Value *> args;
    for (int i = 0; i < node.getParamsCount(); i++) {
        llvm::Value *argVal;

        if (symtab.findScope(node.getParam(i)->getValue())) {
            // Already stored value
            argVal = symtab.getLlvmValue(node.getParam(i)->getValue());
            args.push_back(argVal);
        } else {
            // Expressions and literal values are generated here
            argVal = node.getParam(i)->accept(*this);
            args.push_back(argVal);
        }
    }

    // Returns the function call IR
    return ctx.IRBuilder.CreateCall(callee, args, callee->getReturnType()->isVoidTy() ? "" : "calltmp");
};

llvm::Value *IRGenerator::visit(ReturnNode &node) {
    // Generates the return value
    llvm::Value *ret = node.getStmt()->accept(*this);

    // Return IR statement
    ctx.IRBuilder.CreateRet(ret);

    // Block stack pop
    ctx.popFunction();
    return nullptr;
}

llvm::Value *IRGenerator::visit(IfNode &node) {
    // Evaluates the condition
    llvm::Value *condVal = node.getExpr()->accept(*this);
    if (!condVal) {
        throw std::runtime_error("There is no condition in the if statement");
        return nullptr;
    }

    // Creates a i1 condition
    condVal = ctx.IRBuilder.CreateICmpNE(condVal, llvm::ConstantInt::get(condVal->getType(), 0), "ifcond");

    llvm::Function *function = ctx.IRBuilder.GetInsertBlock()->getParent();

    // Blocks for each part
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(ctx.IRContext, "then", function);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(ctx.IRContext, "else", function);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(ctx.IRContext, "endif", function);

    // Creates the conditional break point
    ctx.IRBuilder.CreateCondBr(condVal, thenBB, elseBB);

    // Generates the if block
    ctx.pushFunction(thenBB);
    node.getCodeBlock()->accept(*this);

    // Inserts the merge block
    if (!ctx.IRBuilder.GetInsertBlock()->getTerminator())
        ctx.IRBuilder.CreateBr(mergeBB);

    ctx.popFunction();

    // Adds the else node if there is one present
    ctx.pushFunction(elseBB);
    if (node.getElseStmt()) {
        node.getElseStmt()->accept(*this);
        ctx.IRBuilder.CreateBr(mergeBB);
    } else {
        ctx.IRBuilder.CreateBr(mergeBB);
    }
    ctx.popFunction();

    // Adds the exit block
    ctx.pushFunction(mergeBB);

    return nullptr;
}

llvm::Value *IRGenerator::visit(ElseNode &node) {
    // Visits the statement
    node.getStmt()->accept(*this);

    return nullptr;
}

llvm::Value *IRGenerator::visit(WhileNode &node) {
    return nullptr;
}

llvm::Value *IRGenerator::visit(ForNode &node) {
    return nullptr;
}
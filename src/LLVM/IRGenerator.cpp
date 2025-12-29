#include "IRGenerator.h"
#include <llvm/IR/Verifier.h>
#include <string.h>

llvm::Value *IRGenerator::visit(CodeBlockNode &node) {
    // Goes to the next scope
    pushScope();

    // Visit all the statement
    for (int i = 0; i < node.getStmtCount(); i++) {
        node.getStmt(i)->accept(*this);
    }

    return nullptr;
}

llvm::Type *IRGenerator::getLlvmType(Type type) {
    if (type.base && type.type == SupportedTypes::TYPE_PTR) {
        return llvm::PointerType::getUnqual(getLlvmType(*type.base));
    }

    switch (type.type) {
    case SupportedTypes::TYPE_INT:
        return llvm::Type::getInt32Ty(ctx.IRContext);
    case SupportedTypes::TYPE_FLOAT:
        return llvm::Type::getFloatTy(ctx.IRContext);
    case SupportedTypes::TYPE_CHAR:
        return llvm::Type::getInt8Ty(ctx.IRContext);
    case SupportedTypes::TYPE_STRING:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0);
    case SupportedTypes::TYPE_BOOL:
        return llvm::Type::getInt1Ty(ctx.IRContext);
    case SupportedTypes::TYPE_VOID:
        return llvm::Type::getVoidTy(ctx.IRContext);
    case SupportedTypes::TYPE_TIME:
        return llvm::Type::getFloatTy(ctx.IRContext);
    default:
        throw std::runtime_error("Unsupported type in IR generation");
    }
};

llvm::Value *IRGenerator::visit(LiteralNode &node) {
    std::variant<int, float, char, std::string, bool> value = node.getVariantValue();

    switch (node.getType().getSupportedType()) {
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
        std::string v = node.getValue();

        // Elimination of the double quote symbols e.g: "a" -> a
        if (v.size() >= 2) {
            v.erase(0, 1);
            v.erase(v.size() - 1, 1);
        }

        // STRING: global string
        return ctx.IRBuilder.CreateGlobalStringPtr(v);
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

llvm::Value *IRGenerator::visit(TimeLiteralNode &node) {
    return llvm::ConstantFP::get(ctx.IRContext, llvm::APFloat(node.getTime()));
}

llvm::Value *IRGenerator::visit(BinaryExprNode &node) {
    Type operationType = node.getType();

    // Left and Right child nodes visit
    llvm::Value *L, *R;
    if (operationType == SupportedTypes::TYPE_STRING) {
        // Generates a UndefValue as placeholder for future constant folding
        L = llvm::UndefValue::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0));
        R = llvm::UndefValue::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0));
    } else {
        L = node.getLeft()->accept(*this);
        R = node.getRight()->accept(*this);
    }

    /* Numeric operations */
    std::string op = node.getValue();

    // FLOAT
    if (operationType == SupportedTypes::TYPE_FLOAT || operationType == SupportedTypes::TYPE_TIME) {
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

    // Catches the error
    std::string errorMsg = "Unsupported binary operation: " + op + " on " + typeToString(operationType);
    errorList.push_back(CompilerError(CompilerPhase::IR_GEN, node.getSourceLocation(), node.getValue(), errorMsg));
    return nullptr;
}

llvm::Value *IRGenerator::visit(UnaryOperationNode &node) {
    llvm::Value *oneLiteral = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.IRContext), 1);
    Symbol *symbol = symtab.getCurrentScope()->getSymbol(node.getValue());

    // Loading the allocated variable
    llvm::Value *alloc = symbol->getLlvmValue();
    llvm::Value *loaded;
    llvm::Type *type;
    if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(alloc)) {
        llvm::Type *type = alloca->getAllocatedType();
        loaded = ctx.IRBuilder.CreateLoad(type, alloca, node.getValue() + "_val");
    }

    llvm::Value *result;

    // Operation
    // FLOAT
    if (symbol->getType() == SupportedTypes::TYPE_FLOAT || symbol->getType() == SupportedTypes::TYPE_TIME) {
        if (node.getOp() == "++")
            result = ctx.IRBuilder.CreateFAdd(loaded, oneLiteral, "addtmp");
        if (node.getOp() == "--")
            result = ctx.IRBuilder.CreateFSub(loaded, oneLiteral, "subtmp");
    }
    // INT
    if (symbol->getType() == SupportedTypes::TYPE_INT || symbol->getType() == SupportedTypes::TYPE_CHAR ||
        symbol->getType() == SupportedTypes::TYPE_BOOL) {
        if (node.getOp() == "++")
            result = ctx.IRBuilder.CreateAdd(loaded, oneLiteral, "addtmp");
        if (node.getOp() == "--")
            result = ctx.IRBuilder.CreateSub(loaded, oneLiteral, "subtmp");
    }

    // Loading the result
    ctx.IRBuilder.CreateStore(result, alloc);

    // If this node is a prefix unary operator the return value is the operation over the variable value
    if (node.isPrefix()) {
        return result;
    }

    // If this node is a postfix unary operator the return value is the value of the variable before the operation
    return loaded;
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
    symtab.getCurrentScope()->getSymbol(node.getValue())->setLlvmValue(allocaInst);

    return allocaInst;
}

llvm::Value *IRGenerator::visit(VariableAssignNode &node) {
    llvm::Value *assignVal;

    // Visits the type of assignment
    if (auto expr = dynamic_cast<BinaryExprNode *>(node.getAssign())) {
        assignVal = visit(*expr);
    } else if (auto lit = dynamic_cast<LiteralNode *>(node.getAssign())) {
        assignVal = visit(*lit);
    } else if (auto lit = dynamic_cast<TimeLiteralNode *>(node.getAssign())) {
        assignVal = visit(*lit);
    } else if (auto unaryOp = dynamic_cast<UnaryOperationNode *>(node.getAssign())) {
        assignVal = visit(*unaryOp);
    } else if (auto var = dynamic_cast<VariableRefNode *>(node.getAssign())) {
        assignVal = visit(*var);

        // Automatic dereferencing the value
        Symbol *assign = symtab.getCurrentScope()->getSymbol(var->getValue());
        if (assign->isPtr()) {
            assignVal = ctx.IRBuilder.CreateLoad(getLlvmType(assign->getType()), assignVal, var->getValue() + "_val");
        }
    } else if (auto fn = dynamic_cast<FunctionCallNode *>(node.getAssign())) {
        assignVal = visit(*fn);
    } else {
        std::string errorMsg = "Not a valid assignment for: " + node.getValue();
        errorList.push_back(CompilerError(CompilerPhase::IR_GEN, node.getSourceLocation(), node.getValue(), errorMsg));
    }

    // Getting the symbol info
    Symbol *symb;
    if (symtab.getCurrentScope()->getSymbol(node.getValue())) {
        symb = symtab.getCurrentScope()->getSymbol(node.getValue());
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
        symb->setLlvmValue(allocaInst);

        // Getting the memory address where the value is stored
        ctx.IRBuilder.CreateStore(assignVal, allocaInst);

        return allocaInst;
    }

    // Gets the memory address and stores the value (ONLY ASSIGNMENT)
    llvm::Value *alloc = symb->getLlvmValue();
    ctx.IRBuilder.CreateStore(assignVal, alloc);

    return alloc;
}

llvm::Value *IRGenerator::visit(VariableRefNode &node) {
    // Getting the symbol data
    Symbol *symbol = symtab.getCurrentScope()->getSymbol(node.getValue());
    if (!symbol) {
        std::string errorMsg = "Symbol not found in scope: " + node.getValue();
        errorList.push_back(CompilerError(CompilerPhase::IR_GEN, node.getSourceLocation(), node.getValue(), errorMsg));
    }

    llvm::Value *alloc = symbol->getLlvmValue();
    if (!alloc) {
        std::string errorMsg = "There is no value associated to the symbol: " + node.getValue();
        errorList.push_back(CompilerError(CompilerPhase::IR_GEN, node.getSourceLocation(), node.getValue(), errorMsg));
    }

    // Returning a direct value
    if (llvm::isa<llvm::Constant>(alloc) || symbol->isPtr()) {
        return alloc;
    }

    // Loading the allocated parameter
    if (llvm::isa<llvm::Argument>(alloc)) {
        llvm::Type *type = alloc->getType();
        // Gets the current function
        llvm::BasicBlock *currentFunction = ctx.blockStack.back();

        // Creates a temporal builder that points to the begin of the current basic block
        llvm::IRBuilder<> tmpBuilder(currentFunction, currentFunction->begin());

        llvm::AllocaInst *allocaInst = tmpBuilder.CreateAlloca(type, nullptr, node.getValue() + "_ptr");
        ctx.IRBuilder.CreateStore(alloc, allocaInst);

        return ctx.IRBuilder.CreateLoad(type, allocaInst, node.getValue() + "_val");
    }

    // Loading the allocated variable
    llvm::Type *type;
    if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(alloc)) {
        llvm::Type *type = alloca->getAllocatedType();
        return ctx.IRBuilder.CreateLoad(type, alloca, node.getValue() + "_val");
    }

    std::string errorMsg = "The symbol: " + node.getValue() + " does not have a valid allocation";
    errorList.push_back(CompilerError(CompilerPhase::IR_GEN, node.getSourceLocation(), node.getValue(), errorMsg));
    return nullptr;
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
    ctx.pushFunction(entry);

    // Setting all the params as arguments in their symbol in the symbol table
    unsigned idx = 0;
    for (auto &arg : function->args()) {
        auto *varNode = dynamic_cast<VariableDecNode *>(node.getParam(idx++));

        // Sets the value of a arg for direct value access
        std::string name = varNode ? varNode->getValue() : "arg" + std::to_string(idx);
        arg.setName(name);

        symtab.getScopeByID(scopeRef + 1)->getSymbol(name)->setLlvmValue(&arg);
    }
    llvm::verifyFunction(*function);

    // IR generation for all the function statements
    node.getCodeBlock()->accept(*this);
    popScope();
    ctx.popFunction();

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

llvm::Value *IRGenerator::generatePrintCall(FunctionCallNode &node) {
    // Getting the arguments values
    std::vector<llvm::Value *> args;
    for (int i = 0; i < node.getParamsCount(); i++) {
        llvm::Value *argVal;

        // Checks if the value is stored
        Symbol *symb = nullptr;
        if (symtab.getCurrentScope()->getSymbol(node.getParam(i)->getValue()) != nullptr) {
            symb = symtab.getCurrentScope()->getSymbol(node.getParam(i)->getValue());
        }

        // Usage of a reference instead of direct value
        if (auto varRef = dynamic_cast<VariableRefNode *>(node.getParam(i))) {
            if (varRef->isRef()) {
                args.push_back(symb->getLlvmValue());
                continue;
            }
        }

        // Expressions, strings and literal values are generated here
        argVal = node.getParam(i)->accept(*this);
        args.push_back(argVal);
    }

    // Pushing the NULL var_arg terminator
    args.push_back(llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0)));

    // Returns the function call IR
    llvm::Function *callee = ctx.IRModule->getFunction(node.getValue());
    return ctx.IRBuilder.CreateCall(callee, args, callee->getReturnType()->isVoidTy() ? "" : "calltmp");
}

llvm::Value *IRGenerator::visit(FunctionCallNode &node) {
    // Function caller
    llvm::Function *callee = ctx.IRModule->getFunction(node.getValue());
    if (!callee) {
        std::string errorMsg = "Undefined function: " + node.getValue();
        errorList.push_back(CompilerError(CompilerPhase::IR_GEN, node.getSourceLocation(), node.getValue(), errorMsg));
    }

    // Built-in function with different generation
    if (node.getValue() == "print") {
        return generatePrintCall(node);
    }

    // Getting the arguments values
    std::vector<llvm::Value *> args;
    for (int i = 0; i < node.getParamsCount(); i++) {
        llvm::Value *argVal;

        // Checks if the value is stored
        Symbol *symb = nullptr;
        if (symtab.getCurrentScope()->getSymbol(node.getParam(i)->getValue()) != nullptr) {
            symb = symtab.getCurrentScope()->getSymbol(node.getParam(i)->getValue());
        }

        // Usage of a reference instead of direct value
        if (auto varRef = dynamic_cast<VariableRefNode *>(node.getParam(i))) {
            if (varRef->isRef()) {
                args.push_back(symb->getLlvmValue());
                continue;
            }
        }

        // Expressions, strings and literal values are generated here
        argVal = node.getParam(i)->accept(*this);
        args.push_back(argVal);
    }

    Symbol *symb = nullptr;
    if (symtab.getCurrentScope()->getSymbol(node.getValue()) != nullptr) {
        symb = symtab.getCurrentScope()->getSymbol(node.getValue());

        if (symb->getCategory() == SymbolCategory::EVENT) {

            llvm::LLVMContext &C = ctx.IRContext;
            llvm::Type *voidTy = llvm::Type::getVoidTy(C);
            llvm::Type *i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(C), 0);

            // Getting the event id
            llvm::Value *eventID = ctx.IRBuilder.CreateGlobalStringPtr(node.getValue(), "event_id");

            // Getting the scheduleEventData function from the module
            llvm::FunctionCallee scheduleFn = ctx.IRModule->getOrInsertFunction(
                "scheduleEventData", llvm::FunctionType::get(voidTy,
                                                             {
                                                                 i8PtrTy,                // const char* id
                                                                 i8PtrTy->getPointerTo() // void** argv
                                                             },
                                                             false));

            // Creating argv in the stack as void* argv[N]
            unsigned argCount = node.getParamsCount();

            llvm::Value *argCountV = llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), argCount);
            llvm::Value *argvAlloca = ctx.IRBuilder.CreateAlloca(i8PtrTy, argCountV, "event_argv");

            // Filling argv[i] with  &valor_real
            for (unsigned i = 0; i < argCount; ++i) {
                llvm::Value *argValue = node.getParam(i)->accept(*this);
                llvm::Value *argAddr = nullptr;

                if (argValue->getType()->isPointerTy()) {
                    argAddr = argValue;
                } else {
                    llvm::Value *tmp = ctx.IRBuilder.CreateAlloca(argValue->getType(), nullptr, "arg_tmp");
                    ctx.IRBuilder.CreateStore(argValue, tmp);
                    argAddr = tmp;
                }

                llvm::Value *argVoidPtr = ctx.IRBuilder.CreateBitCast(argAddr, i8PtrTy);

                // slot = &argv[i]
                llvm::Value *iV = llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), i);
                llvm::Value *slot = ctx.IRBuilder.CreateInBoundsGEP(i8PtrTy, argvAlloca, iV);

                ctx.IRBuilder.CreateStore(argVoidPtr, slot);
            }

            // Calling scheduleEventData
            return ctx.IRBuilder.CreateCall(scheduleFn, {eventID, argvAlloca});
        }
    }

    // Returns the function call IR
    return ctx.IRBuilder.CreateCall(callee, args, callee->getReturnType()->isVoidTy() ? "" : "calltmp");
};

llvm::Value *IRGenerator::visit(ReturnNode &node) {
    // Checks if the return is from a value or void
    if (node.getStmt()) {
        // Generates the return value
        llvm::Value *ret = node.getStmt()->accept(*this);

        // Return IR statement
        ctx.IRBuilder.CreateRet(ret);
    } else {
        // Void return
        ctx.IRBuilder.CreateRetVoid();
    }

    return nullptr;
}

llvm::Value *IRGenerator::visit(IfNode &node) {
    llvm::Function *function = ctx.IRBuilder.GetInsertBlock()->getParent();

    // Blocks for each part
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(ctx.IRContext, "then", function);

    llvm::BasicBlock *elseBB;
    if (node.getElseStmt()) {
        elseBB = llvm::BasicBlock::Create(ctx.IRContext, "else", function);
    }

    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(ctx.IRContext, "endif", function);

    // Creates the conditional break point
    if (node.getElseStmt()) {
        // If the else is present, a false condition jumps to the else block
        ctx.IRBuilder.CreateCondBr(node.getExpr()->accept(*this), thenBB, elseBB);
    } else {
        // If the else is present, a false condition jumps to the endif block
        ctx.IRBuilder.CreateCondBr(node.getExpr()->accept(*this), thenBB, mergeBB);
    }

    // Generates the if block
    ctx.pushFunction(thenBB);
    node.getCodeBlock()->accept(*this);
    popScope();

    // Inserts the merge block if no terminator was found at the end of the if block
    if (!ctx.IRBuilder.GetInsertBlock()->getTerminator()) {
        ctx.IRBuilder.CreateBr(mergeBB);
        ctx.popFunction();
    }

    // Adds the else node if there is one present
    if (node.getElseStmt()) {
        ctx.pushFunction(elseBB);
        node.getElseStmt()->accept(*this);

        if (!ctx.IRBuilder.GetInsertBlock()->getTerminator()) {
            ctx.IRBuilder.CreateBr(mergeBB);
        }
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
    llvm::Function *function = ctx.IRBuilder.GetInsertBlock()->getParent();

    // Blocks for each part
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(ctx.IRContext, "condition", function);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(ctx.IRContext, "loop", function);
    llvm::BasicBlock *endLoopBB = llvm::BasicBlock::Create(ctx.IRContext, "endLoop", function);
    loopContext.condBB = condBB;
    loopContext.endLoopBB = endLoopBB;

    // Jumps to the condition evaluation
    ctx.IRBuilder.CreateBr(condBB);
    ctx.pushFunction(condBB);

    // Creates the conditional break point
    ctx.IRBuilder.CreateCondBr(node.getExpr()->accept(*this), loopBB, endLoopBB);
    ctx.popFunction();

    // Generates the while block
    ctx.pushFunction(loopBB);
    node.getCodeBlock()->accept(*this);
    popScope();

    // Check if the block had a return
    if (!ctx.IRBuilder.GetInsertBlock()->getTerminator()) {
        // At the end of the block jumps to the condition
        ctx.IRBuilder.CreateBr(condBB);
    }

    // Exits the basic block
    ctx.popFunction();

    // The code after the loop must be in the end loop block
    ctx.pushFunction(endLoopBB);

    loopContext.condBB = nullptr;
    loopContext.endLoopBB = nullptr;

    return nullptr;
}

llvm::Value *IRGenerator::visit(ForNode &node) {
    llvm::Function *function = ctx.IRBuilder.GetInsertBlock()->getParent();

    // Blocks for each part
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(ctx.IRContext, "condition", function);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(ctx.IRContext, "loop", function);
    llvm::BasicBlock *endLoopBB = llvm::BasicBlock::Create(ctx.IRContext, "endLoop", function);
    loopContext.condBB = condBB;
    loopContext.endLoopBB = endLoopBB;

    node.getDef()->accept(*this);

    // Jumps to the condition evaluation
    ctx.IRBuilder.CreateBr(condBB);
    ctx.pushFunction(condBB);

    // Creates the conditional break point
    ctx.IRBuilder.CreateCondBr(node.getCondition()->accept(*this), loopBB, endLoopBB);
    ctx.popFunction();

    // Generates the for block
    ctx.pushFunction(loopBB);
    node.getCodeBlock()->accept(*this);
    popScope();

    // Check if the block had a return
    if (!ctx.IRBuilder.GetInsertBlock()->getTerminator()) {
        // Generates the condition variable operation
        node.getAssign()->accept(*this);

        // At the end of the block jumps to the condition
        ctx.IRBuilder.CreateBr(condBB);
    }

    // Exits the basic block
    ctx.popFunction();

    // The code after the loop must be in the end loop block
    ctx.pushFunction(endLoopBB);

    loopContext.condBB = nullptr;
    loopContext.endLoopBB = nullptr;

    return nullptr;
}

llvm::Value *IRGenerator::visit(LoopControlStatementNode &node) {
    if (node.getValue() == "continue") {
        // Jumps to the condition
        ctx.IRBuilder.CreateBr(loopContext.condBB);
    } else {
        // Jumps to the end of the loop
        ctx.IRBuilder.CreateBr(loopContext.endLoopBB);
    }

    return nullptr;
}

auto mapTypeToCode = [](SupportedTypes t) -> int {
    switch (t) {
    case SupportedTypes::TYPE_INT:
        return 1;
    case SupportedTypes::TYPE_FLOAT:
        return 2;
    case SupportedTypes::TYPE_STRING:
        return 3;
    default:
        return 0; // UNKNOWN
    }
};

llvm::Value *IRGenerator::visit(EventNode &node) {
    int paramCount = node.getParamsCount();

    llvm::LLVMContext &C = ctx.IRContext;

    // === Base types ===
    llvm::Type *i8PtrTy = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(C));
    llvm::Type *i32Ty = llvm::Type::getInt32Ty(C);

    // === Arrays LLVM ===
    llvm::ArrayType *argsArrayTy = llvm::ArrayType::get(i8PtrTy, paramCount);
    llvm::ArrayType *typesArrayTy = llvm::ArrayType::get(i32Ty, paramCount);

    // === Type entries ===
    std::vector<llvm::Constant *> typeEntries;

    for (int i = 0; i < paramCount; i++) {
        auto *varNode = dynamic_cast<VariableDecNode *>(node.getParam(i));
        SupportedTypes t = varNode->getType().getSupportedType();

        int code = mapTypeToCode(t);

        typeEntries.push_back(llvm::ConstantInt::get(i32Ty, code));
    }

    llvm::Constant *typesArrayConst = llvm::ConstantArray::get(typesArrayTy, typeEntries);

    llvm::GlobalVariable *typesGlobal = new llvm::GlobalVariable(*ctx.IRModule, typesArrayTy,
                                                                 true, // constant
                                                                 llvm::GlobalValue::PrivateLinkage, typesArrayConst,
                                                                 node.getValue() + "_argtypes" // name
    );

    llvm::Value *typesPtr = ctx.IRBuilder.CreateBitCast(typesGlobal, i32Ty->getPointerTo());

    // Getting the param definition (only types)
    std::vector<llvm::Type *> paramTypes;
    for (int i = 0; i < node.getParamsCount(); i++) {
        if (auto var = dynamic_cast<VariableDecNode *>(node.getParam(i))) {
            paramTypes.push_back(getLlvmType(var->getType()));
        }
    }

    // Event generation
    llvm::Type *returnType = getLlvmType(SupportedTypes::TYPE_VOID);
    llvm::FunctionType *eventType = llvm::FunctionType::get(returnType, paramTypes, false);
    llvm::Function *event = ctx.IRModule->getFunction(node.getValue());

    if (!event) {
        event = llvm::Function::Create(eventType, llvm::Function::ExternalLinkage, node.getValue(), ctx.IRModule.get());
    }

    llvm::Value *eventID = ctx.IRBuilder.CreateGlobalStringPtr(node.getValue(), "str");
    llvm::Value *time = node.getTimeStmt()->accept(*this);
    llvm::Value *limit = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.IRContext), node.getLimit());

    // Inserting the event register function right after the event
    llvm::FunctionCallee fn = ctx.IRModule->getOrInsertFunction(
        "registerEventData", llvm::FunctionType::get(llvm::Type::getVoidTy(C),
                                                     {
                                                         i8PtrTy,                   // id
                                                         llvm::Type::getFloatTy(C), // time
                                                         i8PtrTy,                   // fn pointer
                                                         i32Ty,                     // argCount
                                                         i32Ty->getPointerTo(),     // int* argTypes
                                                         i32Ty                      // limit
                                                     },
                                                     false));

    llvm::Value *fnPtr = ctx.IRBuilder.CreateBitCast(event, i8PtrTy);

    ctx.IRBuilder.CreateCall(fn, {eventID, time, fnPtr, llvm::ConstantInt::get(i32Ty, paramCount), typesPtr, limit});

    // Basic block generation and stack push
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(ctx.IRContext, "entry", event);
    ctx.pushFunction(entry);

    // Setting all the params as arguments in their symbol in the symbol table
    unsigned idx = 0;
    for (auto &arg : event->args()) {
        auto *varNode = dynamic_cast<VariableDecNode *>(node.getParam(idx++));

        // Sets the value of a arg for direct value access
        std::string name = varNode ? varNode->getValue() : "arg" + std::to_string(idx);
        arg.setName(name);

        symtab.getScopeByID(scopeRef + 1)->getSymbol(name)->setLlvmValue(&arg);
    }
    llvm::verifyFunction(*event);

    // IR generation for all the function statements
    node.getCodeBlock()->accept(*this);

    ctx.IRBuilder.CreateRetVoid();
    popScope();

    ctx.popFunction();

    return event;
};

llvm::Value *IRGenerator::visit(ExitNode &node) {
    return nullptr;
};
#include "IRGenerator.h"
#include <llvm/IR/Verifier.h>
#include <string.h>

llvm::Value *IRGenerator::visit(CodeBlockNode &node) {
    // Goes to the next scope
    scopeRef++;
    scopeStack.push_back(scopeRef);
    std::shared_ptr<Scope> scope = symtab.getScopeByID(scopeStack.back());
    if (scope != nullptr) {
        symtab.setCurrentScope(scope);
    }

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
        return llvm::Type::getFP128Ty(ctx.IRContext);
    case SupportedTypes::TYPE_CHAR:
        return llvm::Type::getInt32Ty(ctx.IRContext);
    case SupportedTypes::TYPE_STRING:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx.IRContext), 0);
    case SupportedTypes::TYPE_BOOL:
        return llvm::Type::getInt1Ty(ctx.IRContext);
    case SupportedTypes::TYPE_VOID:
        return llvm::Type::getVoidTy(ctx.IRContext);
    case SupportedTypes::TYPE_TIME:
        return llvm::Type::getFP128Ty(ctx.IRContext);
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
    } else if (auto var = dynamic_cast<VariableRefNode *>(node.getAssign())) {
        assignVal = visit(*var);

        // Automatic dereferencing the value
        Symbol *assign = symtab.getCurrentScope()->getSymbol(node.getAssign()->getValue());
        if (assign->isPtr()) {
            assignVal = ctx.IRBuilder.CreateLoad(getLlvmType(assign->getType()), assignVal,
                                                 node.getAssign()->getValue() + "_val");
        }
    } else if (auto fn = dynamic_cast<FunctionCallNode *>(node.getAssign())) {
        assignVal = visit(*fn);
    } else {
        throw std::runtime_error("Not a valid assigment for: " + node.getValue());
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
        throw std::runtime_error("Symbol not found in scope: " + node.getValue());
    }

    llvm::Value *alloc = symbol->getLlvmValue();
    if (!alloc) {
        throw std::runtime_error("There is no value associated to the symbol: " + node.getValue());
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
    scopeStack.pop_back();
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
        throw std::runtime_error("Undefined function: " + node.getValue());
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

            llvm::Value *eventID = ctx.IRBuilder.CreateGlobalStringPtr(node.getValue(), "str");

            llvm::FunctionCallee fn =
                ctx.IRModule->getOrInsertFunction("scheduleEvent", llvm::FunctionType::get(voidTy, {i8PtrTy}, false));
            return ctx.IRBuilder.CreateCall(fn, eventID, "");
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
    scopeStack.pop_back();

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
    scopeStack.pop_back();

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
    scopeStack.pop_back();

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

llvm::Value *IRGenerator::visit(EventNode &node) {
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

    std::vector<llvm::Value *> args;
    args.push_back(eventID);
    args.push_back(time);
    args.push_back(event);

    // Inserting the event register function right after the event
    llvm::LLVMContext &C = ctx.IRContext;
    llvm::Type *i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(C), 0);
    llvm::Type *voidTy = llvm::Type::getVoidTy(C);
    llvm::Type *float64Ty = llvm::Type::getFloatTy(C);
    llvm::Type *fnPtrTy = llvm::PointerType::getUnqual(llvm::FunctionType::get(voidTy, false));
    llvm::FunctionCallee fn = ctx.IRModule->getOrInsertFunction(
        "registerEventData", llvm::FunctionType::get(voidTy, {i8PtrTy, float64Ty, fnPtrTy}, false));
    ctx.IRBuilder.CreateCall(fn, args, "");

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
    scopeStack.pop_back();

    ctx.popFunction();

    return event;
};

llvm::Value *IRGenerator::visit(ExitNode &node) {
    return nullptr;
};
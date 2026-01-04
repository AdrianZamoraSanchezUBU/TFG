/**
 * @file CodegenContext.h
 * @brief Contains the definition of a LLVM IR program package.
 *
 * @author Adrián Zamora Sánchez
 */

#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

/// Structure that contains the LLVM context of a compiled program.
struct CodegenContext {
    /// LLVM context.
    llvm::LLVMContext IRContext;

    /// IR builder.
    llvm::IRBuilder<> IRBuilder;

    /// Module of the program to generate.
    std::unique_ptr<llvm::Module> IRModule;

    /// Stack of IR code blocks.
    std::vector<llvm::BasicBlock *> blockStack;

    /// Module and IRBuilder set up.
    explicit CodegenContext() : IRBuilder(IRContext), IRModule(std::make_unique<llvm::Module>("program", IRContext)) {
        llvm::LLVMContext &C = IRContext;
        llvm::Type *i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(C), 0);
        llvm::Type *i32Ty = llvm::Type::getInt32Ty(C);
        llvm::Type *voidTy = llvm::Type::getVoidTy(C);
        llvm::Type *floatTy = llvm::Type::getFloatTy(C);

        llvm::FunctionType *callbackTy = llvm::FunctionType::get(voidTy, false);
        llvm::PointerType *callbackPtrTy = llvm::PointerType::getUnqual(callbackTy);

        // Inserta la función C si no existe
        IRModule->getOrInsertFunction("intToString", llvm::FunctionType::get(i8PtrTy, {i32Ty}, false));
        IRModule->getOrInsertFunction("floatToString", llvm::FunctionType::get(i8PtrTy, {floatTy}, false));
        IRModule->getOrInsertFunction("print", llvm::FunctionType::get(voidTy, {i8PtrTy}, true));
        IRModule->getOrInsertFunction("strlen", llvm::FunctionType::get(i32Ty, {i8PtrTy}, false));
        IRModule->getOrInsertFunction("registerEventData",
                                      llvm::FunctionType::get(voidTy,
                                                              {
                                                                  i8PtrTy,               // id
                                                                  floatTy,               // time
                                                                  i8PtrTy,               // fn pointer
                                                                  i32Ty,                 // argCount
                                                                  i32Ty->getPointerTo(), // int* argTypes
                                                                  i32Ty                  // limit
                                                              },
                                                              false));
        IRModule->getOrInsertFunction("scheduleEvent",
                                      llvm::FunctionType::get(voidTy,
                                                              {
                                                                  i8PtrTy,                // const char* id
                                                                  i8PtrTy->getPointerTo() // void** argv
                                                              },
                                                              false));

        // Program main function and basic block set up
        llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(IRContext), false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "mainLLVM", IRModule.get());
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(IRContext, "entry", F);

        // Sets the current basic block in the stack
        IRBuilder.SetInsertPoint(BB);
        pushFunction(BB);
    }

    /**
     * @brief Pushes a new basic block to the stack and sets it as the new insert point.
     * @param BB New basic block.
     */
    void pushFunction(llvm::BasicBlock *BB) {
        blockStack.push_back(BB);
        IRBuilder.SetInsertPoint(BB);
    }

    /// Sets the current insert point to the next basic block in the stack.
    void popFunction() {
        // Checks if is at the bottom of the stack
        if (blockStack.empty()) {
            std::runtime_error("Error: function stack empty at pop operation");
        } else {
            blockStack.pop_back();
        }

        if (!blockStack.empty()) {
            auto &prev = blockStack.back();

            if (prev == nullptr) {
                std::runtime_error("Error: function stack previous insert point is null");
            }

            IRBuilder.SetInsertPoint(prev);
        }
    }
};

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

/**
 * @brief Structure that contains the LLVM context of a compiled program.
 */
struct CodegenContext {
    /// LLVM context.
    llvm::LLVMContext IRContext;

    /// IR builder.
    llvm::IRBuilder<> IRBuilder;

    /// Module of the program to generate.
    std::unique_ptr<llvm::Module> IRModule;

    /// Stack of IR code blocks.
    std::vector<llvm::BasicBlock *> blockStack;

    /**
     * @brief Module and IRBuilder set up.
     */
    explicit CodegenContext() : IRBuilder(IRContext), IRModule(std::make_unique<llvm::Module>("program", IRContext)) {
        // Program main function and basic block set up
        llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(IRContext), false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", IRModule.get());
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

    /**
     * @brief Sets the current insert point to the next basic block in the stack.
     */
    void popFunction() {
        blockStack.pop_back();

        if (!blockStack.empty()) {
            auto &prev = blockStack.back();
            IRBuilder.SetInsertPoint(prev);
        }
    }
};

/**
 * @file CodegenContext.h
 * @brief Contains the definition of a LLVM program package.
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
	/// @brief LLVM context.
	llvm::LLVMContext IRContext;

	/// @brief IR builder.
	llvm::IRBuilder<> IRBuilder;

	/// @brief Module of the program to generate.
	std::unique_ptr<llvm::Module> IRModule;

	/**
	 * @brief Module and IRBuilder set up.
	 */
	CodegenContext()
		: IRBuilder(IRContext), 
		  IRModule(std::make_unique<llvm::Module>("program", IRContext))
	{}
};

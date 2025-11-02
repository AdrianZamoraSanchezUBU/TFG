/**
 * @file IRGenerator.h
 * @brief Contains the definition of the LLVM IR generator.
 *
 * Generates the LLVM IR from the information contained in the AST.
 *
 * @author Adrián Zamora Sánchez
 * @see CodegenContext.h
 * @see AST.h
 */

#pragma once
#include "AST.h"
#include "CodegenContext.h"
#include "SupportedTypes.h"
#include "SymbolTable.h"

class LiteralNode;
class BinaryExprNode;

/**
 * @class IRGenerator
 * @brief Generates IR code using the visitor pattern.
 *
 * This class visits AST nodes, generating for each visit
 * the IR code associated with the information of the visited node.
 *
 * @see ASTNode
 * @see CodegenContext
 */
class IRGenerator {
    CodegenContext ctx;  /// LLVM Context.
    SymbolTable &symtab; /// Symbol Table reference.

  public:
    /**
     * @brief IRGenerator constructor.
     *
     * Initializes the LLVM context.
     */
    explicit IRGenerator(SymbolTable &table) : ctx(), symtab(table){};

    /**
     * @brief Returns the llvm::Type associated with a SupportedTypes type.
     * @param type SupportedType format.
     * @return llvm::Type* format of the type.
     */
    llvm::Type *getLlvmType(SupportedTypes);

    /**
     * @return Returns the LLVM Context.
     */
    CodegenContext &getContext() { return ctx; }

    /**
     * @brief Visits a code block node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(CodeBlockNode &node);

    /**
     * @brief Visits a literal node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(LiteralNode &node);

    /**
     * @brief Visits a binary expression node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(BinaryExprNode &node);

    /**
     * @brief Visits a variable declaration node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(VariableDecNode &node);

    /**
     * @brief Visits a variable assignment node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(VariableAssignNode &node);

    /**
     * @brief Visits a variable reference node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(VariableRefNode &node);

    /**
     * @brief Visit a function definition node.
     * @param node Node to be visited.
     */
    llvm::Value *visit(FunctionDefNode &node);

    /**
     * @brief Visit a function declaration node.
     * @param node Node to be visited.
     */
    llvm::Value *visit(FunctionDecNode &node);

    /**
     * @brief Visit a function call node.
     * @param node Node to be visited.
     */
    llvm::Value *visit(FunctionCallNode &node);

    /**
     * @brief Visits a return statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(ReturnNode &node);
};
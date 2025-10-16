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
    /// @brief LLVM Context.
    CodegenContext ctx;

  public:
    /**
     * @brief IRGenerator constructor.
     *
     * Initializes the LLVM context.
     */
    IRGenerator();

    /**
     * @return Returns the LLVM Context.
     */
    CodegenContext &getContext() { return ctx; }

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
};
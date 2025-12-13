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
    CodegenContext ctx;                    /// LLVM Context.
    SymbolTable &symtab;                   /// Symbol Table reference.
    std::vector<int> scopeStack;           /// Scope stack
    int scopeRef = -1;                     /// The scope is -1 before the main program initialization
    std::vector<CompilerError> &errorList; /// List of language misuses

    /**
     * @brief Matches the condition and end of a loop.
     *
     * This structure allow safe nested loop management.
     */
    struct LoopContext {
        llvm::BasicBlock *condBB;
        llvm::BasicBlock *endLoopBB;
    };
    LoopContext loopContext;

  public:
    /**
     * @brief IRGenerator constructor.
     *
     * Initializes the LLVM context.
     */
    explicit IRGenerator(SymbolTable &table, std::vector<CompilerError> &err) : ctx(), symtab(table), errorList(err){};

    /// Inserts a new scope ID in the stack.
    void pushScope() {
        scopeRef++;
        scopeStack.push_back(scopeRef);

        // Sets the new scope as current scope
        std::shared_ptr<Scope> scope = symtab.getScopeByID(scopeStack.back());
        symtab.setCurrentScope(scope);
    }

    /// Removes the top scope ID from the stack and sets the previous scope.
    void popScope() {
        if (!scopeStack.empty()) {
            scopeStack.pop_back();

            // Sets the previous scope as current scope
            std::shared_ptr<Scope> scope = symtab.getScopeByID(scopeStack.back());
            symtab.setCurrentScope(scope);
        } else {
            throw std::runtime_error("Scope stack empty");
        }
    }

    /**
     * @brief Returns the llvm::Type associated with a Type.
     * @param type Type format.
     * @return llvm::Type* format of the type.
     */
    llvm::Type *getLlvmType(Type);

    /**
     * @return Returns the LLVM Context.
     * @return structure with the LLVM IR generation data.
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
     * @brief Visits a time literal node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(TimeLiteralNode &node);

    /**
     * @brief Visits a binary expression node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(BinaryExprNode &node);

    /**
     * @brief Visits a unary operation node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(UnaryOperationNode &node);

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
     * @brief Built-in function call node visit.
     * @param node Node with a "print" function call.
     */
    llvm::Value *generatePrintCall(FunctionCallNode &node);

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

    /**
     * @brief Visits a if statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(IfNode &node);

    /**
     * @brief Visits a else statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(ElseNode &node);

    /**
     * @brief Visits a while loop statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(WhileNode &node);

    /**
     * @brief Visits a for loop statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(ForNode &node);

    /**
     * @brief Visits a loop control statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(LoopControlStatementNode &node);

    /**
     * @brief Visits a event statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(EventNode &node);

    /**
     * @brief Visits a exit statement node.
     * @param node Node to be visited.
     * @return llvm::Value* Value obtained from the visit.
     */
    llvm::Value *visit(ExitNode &node);
};
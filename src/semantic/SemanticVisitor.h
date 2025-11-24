/**
 * @file SemanticVisitor.h
 * @brief Visitor for the AST structure that creates the SymbolTable.
 *
 * @author Adrián Zamora Sánchez
 */
#pragma once
#include "AST.h"
#include "SymbolTable.h"

/**
 * @brief Visitor pattern that builds the SymbolTable from the AST.
 * @see AST.h
 */
class SemanticVisitor {
    SymbolTable &symtab;
    unsigned int loopDepth = 0;

  public:
    explicit SemanticVisitor(SymbolTable &table) : symtab(table) {}

    /**
     * @brief Visits the literal node.
     * @param node Node to be visited.
     */
    void *visit(LiteralNode &node);

    /**
     * @brief Visits the time literal
     * @param node Node to be visited.
     */
    void *visit(TimeLiteralNode &node);

    /**
     * @brief Checks the correctness of a binary expression node.
     * @param node Node to be visited.
     */
    void *visit(BinaryExprNode &node);

    /**
     * @brief Visits a code block node.
     * @param node Node to be visited.
     */
    void *visit(CodeBlockNode &node);

    /**
     * @brief Visits a variable declaration node.
     * @param node Node to be visited.
     */
    void *visit(VariableDecNode &node);

    /**
     * @brief Visits a variable declaration node.
     * @param node Node to be visited.
     */
    void *visit(VariableAssignNode &node);

    /**
     * @brief Visits a variable reference node.
     * @param node Node to be visited.
     */
    void *visit(VariableRefNode &node);

    /**
     * @brief Visit a function definition node.
     * @param node Node to be visited.
     */
    void *visit(FunctionDefNode &node);

    /**
     * @brief Visit a function declaration node.
     * @param node Node to be visited.
     */
    void *visit(FunctionDecNode &node);

    /**
     * @brief Visit a function call node.
     * @param node Node to be visited.
     */
    void *visit(FunctionCallNode &node);

    /**
     * @brief Visits a return statement node.
     * @param node Node to be visited.
     */
    void *visit(ReturnNode &node);

    /**
     * @brief Visits a if statement node.
     * @param node Node to be visited.
     */
    void *visit(IfNode &node);

    /**
     * @brief Visits a else statement node.
     * @param node Node to be visited.
     */
    void *visit(ElseNode &node);

    /**
     * @brief Visits a while loop statement node.
     * @param node Node to be visited.
     */
    void *visit(WhileNode &node);

    /**
     * @brief Visits a for loop statement node.
     * @param node Node to be visited.
     */
    void *visit(ForNode &node);

    /**
     * @brief Visit a loop control statement (continue or break) node.
     * @param node Node to be visited.
     */
    void *visit(LoopControlStatementNode &node);

    /**
     * @brief Visits a event statement node.
     * @param node Node to be visited.
     */
    void *visit(EventNode &node);

    /**
     * @brief Visits a exit statement node.
     * @param node Node to be visited.
     */
    void *visit(ExitNode &node);

    /**
     * @brief Prints the content of the SymbolTable.
     */
    void printSymbolTable() const { symtab.print(); }
};
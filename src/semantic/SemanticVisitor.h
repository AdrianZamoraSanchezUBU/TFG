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

  public:
    SemanticVisitor(SymbolTable &table) : symtab(table) {}

    /**
     * @brief Needed visit
     * @param node Node to be visited.
     */
    void *visit(LiteralNode &node);

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
     * @brief Prints the content of the SymbolTable.
     */
    void printSymbolTable() { symtab.print(); }
};
#pragma once
#include "CodegenContext.h"
#include "AST.h"

class LiteralNode;
class BinaryExprNode;

/**
 * @class IRGenerator
 * @brief Genera el código IR con visitor pattern.
 * 
 * Esta clase visita los nodos del AST generando para 
 * cada visita el código IR asociado a la información y tipo
 * del nodo visitado.
 *
 * @see ASTNode
 * @see CodegenContext
 */
class IRGenerator {
    /// @brief  Contexto de LLVM
    CodegenContext ctx;
public:
    /**
     * @brief Constructor de IRGenerator
     * 
     * Inicializa el LLVM context
     */
    IRGenerator();

    /**
     * @return Devuelve el contexto de LLVM 
     */
    CodegenContext& getContext() { return ctx; }

    /**
     * @brief Visita un nodo literal
     * @param node nodo que se recibe para su visita
     * @return llvm::Value* Valor obtenido de la visita
     */
    llvm::Value* visit(LiteralNode& node);

    /**
     * @brief Visita un nodo de expresión binaria
     * @param node nodo que se recibe para su visita 
     * @return llvm::Value* Valor obtenido de la visita
     */ 
    llvm::Value* visit(BinaryExprNode& node);
};
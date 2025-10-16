#pragma once
#include "TParser.h"

// Forward declaration
class ASTNode;

/**
 * @class ASTBuilder
 * @brief Genera el AST con visitor pattern.
 * 
 * Esta clase visita los contextos del programa de forma ordenada
 * generando para cada visita un nodo y de esta forma construyendo
 * el AST completo.
 *
 * @see ASTNode
 */
class ASTBuilder {
public:
	/**
	 * @brief Visita el programa y devuelve los nodos que contiene.
	 * @param ctx Contexto del programa base.
	 * @return Vector con los nodos del programa.
	 */
	std::vector<std::unique_ptr<ASTNode>> visit(TParser::ProgramContext* ctx);

	/**
	 * @brief Visita el contexto de un statement.
	 * @param ctx Contexto de un statement.
	 * @return Nodo del AST que representa ese statement.
	 */
	std::unique_ptr<ASTNode> visit(TParser::StmtContext* ctx);

	/**
	 * @brief Visita el contexto de las expresiones aritméticas y lógicas.
	 *
	 * Visita el contexto de las expresiones aritméticas y lógicas, 
	 * sirve como dispatcher para visitar los diferentes tipos de expresiones.
	 * 
	 * @param ctx Contexto de expresión.
	 * @return Nodo del AST asociado a la expresión.
	 */
	std::unique_ptr<ASTNode> visit(TParser::ExprContext *ctx);

	/**
	 * @brief Visita el contexto de un operador de la expresión.
	 * @param ctx Contexto de operador.
	 * @return Nodo del AST asociado a un operador.
	 */
	std::unique_ptr<ASTNode> visit(TParser::OperandExprContext *ctx);

	/**
	 * @brief Visita el contexto de una expresión aritmética.
	 * @param ctx Contexto de expresión aritmética.
	 * @return Nodo del AST asociado a la expresión.
	 */
	std::unique_ptr<ASTNode> visit(TParser::ArithmeticExprContext *ctx);

	/**
	 * @brief Visita el contexto de una expresión lógica. 
	 * @param ctx Contexto de expresión lógica.
	 * @return Nodo del AST asociado a la expresión.
	 */
	std::unique_ptr<ASTNode> visit(TParser::LogicalExprContext *ctx);

	/**
	 * @brief Visita el contexto de un literal.
	 * @param ctx Contexto de un literal.
	 * @return Nodo que representa un valor literal en el código.
	 */
	std::unique_ptr<ASTNode> visit(TParser::LiteralContext *ctx);
};

#pragma once
#include <string>
#include <memory>
#include <variant>

// Forward declarations
class IRGenerator;
namespace llvm { class Value; }

/**
 * @class ASTNode
 * @brief Representa un nodo genérico del AST.
 * 
 * Esta clase es una representación genérica de los nodos 
 * que conforman el AST.
 */
class ASTNode{
private:
public:
	virtual ~ASTNode() = default;

	/**
	 * @brief Devuelve un string con el valor del nodo.
	 * @return Cadena con el valor del nodo.
	 */
	virtual std::string getValue() const { return ""; };

	/**
	 * @brief Compara el propio nodo con otro para verificar si son iguales.
	 * @param other Puntero a otro nodo.
	 * @return `true` si son iguales, `false` en caso contrario.
	 */
	virtual bool equals(const ASTNode* other) const = 0;

	/**
	 * @brief Accept del patron visitor de IRGenerator
	 * @param visitor Objeto encargado de generar y almacenar el LLVM IR
	 * @return Valor de LLVM (llvm::Value*)
	 * @see IRGenerator
	 */
	virtual llvm::Value* accept(IRGenerator& visitor) = 0;
};

/**
 * @class LiteralNode
 * @brief Representa un literal del AST.
 * 
 * Esta clase representa un valor literal dentro del AST,
 * este puede ser de los tipos soportados.
 * 
 * @see ASTNode
 */
class LiteralNode : public ASTNode {
private:
	std::variant<int, float, char, std::string, bool> value;
public:
	/**
     * @brief Constructor del nodo literal.
     * @param val Valor literal (int, float, char, string o bool).
     */
	explicit LiteralNode(std::variant<int, float, char, std::string, bool> val) 
	: value(std::move(val)) {}

	/**
	 * @brief Devuelve el valor como std::variant
	 */
	const std::variant<int, float, char, std::string, bool>& getVariantValue() const {
        return value;
    }

	/// @copydoc ASTNode::getValue
	std::string getValue() const override {
		// Tries casting to every possible type
		if (auto p = std::get_if<int>(&value))
	    	return std::to_string(*p);
		if (auto p = std::get_if<float>(&value))
		    return std::to_string(*p);
		if (auto p = std::get_if<char>(&value))
		    return std::string(1, *p);
		if (auto p = std::get_if<std::string>(&value))
		    return *p;
		if (auto p = std::get_if<bool>(&value))
		    return *p ? "true" : "false";

		return ""; // Unreachable
    }

	/// @copydoc ASTNode::equals
	bool equals(const ASTNode* other) const override {
		// Dynamic cast to LiteralNode and value check 
		if (auto o = dynamic_cast<const LiteralNode*>(other)) {
            return value == o->value;
        }

        return false;
	}

	/// @copydoc ASTNode::accept
	llvm::Value* accept(IRGenerator& visitor) override;
};

/**
 * @class BinaryExprNode
 * @brief Representa una expresión aritmética o lógica en el AST.
 * 
 * Esta clase representa una expresión aritmética o lógica donde
 * sus dos nodos hijos son operandos.
 * 
 * @see ASTNode
 */
class BinaryExprNode : public ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

public:
	/**
     * @brief Constructor del nodo literal.
     * @param op Operador artimético o lógico.
     * @param lhs Operando izquierdo.
     * @param rhs Operando derecho.
     */
    explicit BinaryExprNode(const std::string& op,
                   std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs)
        		   : op(op), left(std::move(lhs)), right(std::move(rhs)) {}

	/**
	 * @brief Devuelve el operador asociado al nodo.
	 * @return Operador en formato string.
	 */
    std::string getValue() const { return op; }

    /**
     * @brief Devuelve el nodo hijo izquierdo.
     * @return Puntero al nodo hijo izquierdo.
     */
   	ASTNode* getLeft() const { return left.get(); }

   	 /**
      * @brief Devuelve el nodo hijo derecho.
      * @return Puntero al nodo hijo derecho.
      */
   	ASTNode* getRight() const { return right.get(); }

	/// @copydoc ASTNode::equals
   	bool equals(const ASTNode* other) const override {
   		// Dynamic cast to BinaryExprNode and value check
        if (auto o = dynamic_cast<const BinaryExprNode*>(other)) {
			// Returns a comparison of their operator, lhs and rhs
            return op == o->op &&
                   left->equals(o->getLeft()) && 
                   right->equals(o->getRight());
        }
        
        return false;
    }

	/// @copydoc ASTNode::accept
	llvm::Value* accept(IRGenerator& visitor) override;
};

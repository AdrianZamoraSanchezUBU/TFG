#include <string>
#include <memory>
#include <variant>

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
};

/**
 * @class literalNode
 * @brief Representa un literal del AST.
 * 
 * Esta clase representa un valor literal dentro del AST,
 * este puede ser de los tipos soportados.
 * 
 * @see ASTNode
 */
class literalNode : public ASTNode {
private:
	std::variant<int, float, char, std::string, bool> value;
public:
	/**
     * @brief Constructor del nodo literal.
     * @param val Valor literal (int, float, char, string o bool).
     */
	explicit literalNode(std::variant<int, float, char, std::string, bool> val) 
	: value(std::move(val)) {}

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
		// Dynamic cast to literalNode and value check 
		if (auto o = dynamic_cast<const literalNode*>(other)) {
            return value == o->value;
        }

        return false;
	}
};

// Class for binary expressions (arithmetic and logical expressions)
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
    BinaryExprNode(const std::string& op,
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
};

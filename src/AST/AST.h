/**
 * @file AST.h
 * @brief Contains the definition of the Abstract Syntax Tree (AST).
 * 
 * @author Adrián Zamora Sánchez
 */

#pragma once
#include <string>
#include <memory>
#include <variant>

// Forward declarations
class IRGenerator;
namespace llvm { class Value; }

/**
 * @class ASTNode
 * @brief Represents a generic AST node.
 * 
 * This class is a generic representation of the nodes 
 * that make up the AST.
 */
class ASTNode{
private:
public:
	virtual ~ASTNode() = default;

	/**
	 * @brief Returns the value in string format.
	 * @return String with the value of this node.
	 */
	virtual std::string getValue() const { return ""; };

	/**
	 * @brief Compares this node with another to check if they are equal.
	 * @param other Pointer to another node.
	 * @return `true` if they are equal, `false` otherwise.
	 */
	virtual bool equals(const ASTNode* other) const = 0;

	/**
	 * @brief Accept method of the IRGenerator visitor pattern
	 * @param visitor Object responsible for generating and storing the LLVM IR
	 * @return LLVM value (llvm::Value*)
	 * @see IRGenerator
	 */
	virtual llvm::Value* accept(IRGenerator& visitor) = 0;
};

/**
 * @class LiteralNode
 * @brief Represents a literal in the AST.
 * 
 * This class represents a literal value within the AST,
 * which can be of any of the supported types.
 * 
 * @see ASTNode
 */
class LiteralNode : public ASTNode {
private:
	std::variant<int, float, char, std::string, bool> value;
public:
	/**
	 * @brief Constructor for the literal node.
	 * @param val Value associated with the node.
	 */
	explicit LiteralNode(std::variant<int, float, char, std::string, bool> val) 
	: value(std::move(val)) {}

	/**
	 * @brief Returns the value as std::variant.
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
 * @brief Represents an arithmetic or logical expression in the AST.
 * 
 * This class represents an arithmetic or logical expression where
 * its two child nodes are operands.
 * 
 * @see ASTNode
 */
class BinaryExprNode : public ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

public:
	/**
	 * @brief Constructor for the binary expr node.
	 * @param op Arithmetic or logical operator.
	 * @param lhs Left-hand operand.
	 * @param rhs Right-hand operand.
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
     * @brief Returns the left hand side operator.
     * @return Returns a pointer to the left hand side node.
     */
   	ASTNode* getLeft() const { return left.get(); }

   	/**
     * @brief Returns the right hand side operator.
     * @return Returns a pointer to the right hand side node.
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

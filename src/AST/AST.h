#include <string>
#include <memory>
#include <variant>

class ASTNode{
private:
public:
	virtual ~ASTNode() = default;
	virtual std::string getValue() const { return ""; };
	virtual bool equals(const ASTNode* other) const = 0;
};

// Class for literals
class literalNode : public ASTNode {
private:
	std::variant<int, float, char, std::string, bool> value;
public:
	explicit literalNode(std::variant<int, float, char, std::string, bool> val) 
	: value(std::move(val)) {}

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
    BinaryExprNode(const std::string& op,
                   std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs)
        		   : op(op), left(std::move(lhs)), right(std::move(rhs)) {}

    std::string getOperator() const { return op; }
   	ASTNode* getLeft() const { return left.get(); }
   	ASTNode* getRight() const { return right.get(); }

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

#include <string>
#include <memory>

class ASTNode{
private:
public:
	virtual ~ASTNode() = default;
	virtual std::string getValue() const { return ""; };
	virtual bool equals(const ASTNode* other) const = 0;
};

class LiteralIntNode : public ASTNode{
	int value;
public:
	LiteralIntNode(int val) : value(val) {}
	
	std::string getValue() const { return std::to_string(value); }

	bool equals(const ASTNode* other) const override {
		// Dynamic cast to LiteralIntNode and value check 
		if (auto o = dynamic_cast<const LiteralIntNode*>(other)) {
            return getValue() == o->getValue();
        }

        return false;
	}
};

class LiteralFloatNode : public ASTNode {
	float value;
public:
	LiteralFloatNode(float val) : value(val) {}

	std::string getValue() const { return std::to_string(value); }

	bool equals(const ASTNode* other) const override {
		// Dynamic cast to LiteralFloatNode and value check 
		if (auto o = dynamic_cast<const LiteralFloatNode*>(other)) {
            return getValue() == o->getValue();
        }

        return false;
	}
};

class LiteralStringNode : public ASTNode {
	std::string value;
public:
	LiteralStringNode(std::string val) : value(val) {}

	std::string getValue() const { return value; }

	bool equals(const ASTNode* other) const override {
		// Dynamic cast to LiteralStringNode and value check 
		if (auto o = dynamic_cast<const LiteralStringNode*>(other)) {
            return getValue() == o->getValue();
        }

        return false;
	}
};

class LiteralBooleanNode : public ASTNode{
	bool value;
public:
	LiteralBooleanNode(bool val) : value(val) {}

	std::string getValue() const {
		if(value){
			return "true";
		} 

		return "false";
	}

	bool equals(const ASTNode* other) const override {
		// Dynamic cast to LiteralBooleanNode and value check 
		if (auto o = dynamic_cast<const LiteralBooleanNode*>(other)) {
            return getValue() == o->getValue();
        }

        return false;
	}
};

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
            return getOperator() == o->getOperator() &&
                   left->equals(o->getLeft()) && 
                   right->equals(o->getRight());
        }
        return false;
    }
};

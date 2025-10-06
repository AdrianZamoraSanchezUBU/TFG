#include <string>
#include <memory>

class ASTNode{
private:
public:
	virtual ~ASTNode() = default;

	virtual std::string getValue(){return "";};
};

class LiteralIntNode : public ASTNode{
	int value;
public:
	LiteralIntNode(int val) : value(val) {}
	
	std::string getValue() const{ return std::to_string(value); }
};

class LiteralFloatNode : public ASTNode{
	float value;
public:
	LiteralFloatNode(float val) : value(val) {}

	std::string getValue() const{ return std::to_string(value); }
};

class LiteralStringNode : public ASTNode{
	std::string value;
public:
	LiteralStringNode(std::string val) : value(val) {}

	std::string getValue() const{ return value; }
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

    std::string getOperator() const{ return op; }
   	ASTNode* getLeft() const { return left.get(); }
   	ASTNode* getRight() const { return right.get(); }
};

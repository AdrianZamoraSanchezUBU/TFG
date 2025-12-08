/**
 * @file AST.h
 * @brief Contains the definition of the Abstract Syntax Tree (AST) nodes.
 *
 * @author Adrián Zamora Sánchez
 */

#pragma once
#include "TimeStamp.h"
#include "Type.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

// Forward declarations
class IRGenerator;
class SemanticVisitor;
namespace llvm {
class Value;
class Function;
} // namespace llvm

/**
 * @class ASTNode
 * @brief Represents a generic AST node.
 *
 * This class is a generic representation of the nodes
 * that make up the AST.
 */
class ASTNode {
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
    virtual bool equals(const ASTNode *other) const = 0;

    /**
     * @brief Accept method of the IRGenerator visitor pattern
     * @param visitor Object responsible for generating and storing the LLVM IR
     * @return LLVM value (llvm::Value*)
     * @see IRGenerator
     */
    virtual void *accept(SemanticVisitor &visitor) = 0;

    /**
     * @brief Accept method of the IRGenerator visitor pattern
     * @param visitor Object responsible for generating and storing the LLVM IR
     * @return LLVM value (llvm::Value*)
     * @see IRGenerator
     */
    virtual llvm::Value *accept(IRGenerator &visitor) = 0;

    /**
     * @brief Prints data about this node
     * @return string with this node in LaTex forest notation.
     */
    virtual std::string print() const = 0;
};

/**
 * @class CodeBlockNode
 * @brief Represents a block of statements in the AST.
 *
 * This class represents a block of code in the AST,
 * which can contain other statements. It is present in
 * functions, loops and control structures.
 *
 * @see ASTNode
 */
class CodeBlockNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;

  public:
    /**
     * @brief Constructor for the CodeBlock node.
     * @param stmt Nodes associated to the statement of this block.
     */
    explicit CodeBlockNode(std::vector<std::unique_ptr<ASTNode>> stmt) : statements(std::move(stmt)) {}

    /**
     * @brief Returns the ammount of statements in this block.
     * @return amount of statements in this block.
     */
    int getStmtCount() const { return statements.size(); }

    /**
     * @brief Returns the statement with index i.
     * @return Statement with index i.
     */
    ASTNode *getStmt(int i) const { return statements[i].get(); }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        // Dynamic cast to LiteralNode and value check
        if (auto o = dynamic_cast<const CodeBlockNode *>(other)) {
            for (int i = 0; i < statements.size(); i++) {
                if (!getStmt(i)->equals(o->getStmt(i))) {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    /// @copydoc ASTNode::print
    std::string print() const override {
        std::string blockString;
        for (int i = 0; i < getStmtCount(); i++) {
            blockString.append(getStmt(i)->print());
        }

        return blockString;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
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
    std::variant<int, float, char, std::string, bool> value;
    Type type;

  public:
    /**
     * @brief Constructor for the literal node.
     * @param val Value associated with the node.
     * @param t type of this literal.
     */
    explicit LiteralNode(std::variant<int, float, char, std::string, bool> val, Type t)
        : value(std::move(val)), type(t) {}

    /**
     * @brief Returns the value as std::variant.
     * @return reference to the std::variant value contained in the node.
     */
    const std::variant<int, float, char, std::string, bool> &getVariantValue() const { return value; }

    /**
     * @brief Getter for type.
     * @return Type of the node.
     */
    Type getType() const { return type; }

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

    /// @copydoc ASTNode::print
    std::string print() const override {
        std::string text = getValue();

        // replacing % for \\% in order to fit printf macros
        size_t pos = 0;
        while ((pos = text.find('%', pos)) != std::string::npos) {
            text.replace(pos, 1, "\\%");
            pos += 2;
        }

        return "\n[{" + text + "},literalNode]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        // Dynamic cast to LiteralNode and value check
        if (auto o = dynamic_cast<const LiteralNode *>(other)) {
            return (value == o->value && type == o->type);
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class TimeLiteralNode
 * @brief Represents a literal in the AST.
 *
 * This class represents a literal value within the AST,
 * which can be of any of the supported types.
 *
 * @see ASTNode
 */
class TimeLiteralNode : public ASTNode {
    float value;
    TimeStamp type;

  public:
    /**
     * @brief Constructor for the time literal node.
     * @param val Value associated with the node.
     * @param t Time unit defined for this variable.
     */
    explicit TimeLiteralNode(float val, TimeStamp t) : value(val), type(t) {}

    /**
     * @brief Getter for value.
     * @return Float type amount of ticks.
     */
    float getTime() const { return value; }

    /**
     * @brief Getter for type.
     * @return Time unit representation for this time value.
     */
    TimeStamp getTimeStamp() const { return type; }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return timeToString(type); }

    /**
     * @brief Setter for value.
     * @param val Amount of time ticks.
     */
    void setValue(float val) { value = val; }

    /**
     * @brief Setter for time stamp.
     * @param newTimeType Time unit of this node.
     */
    void setTime(TimeStamp newTimeType) { type = newTimeType; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        return "[" + std::to_string(value) + " " + getValue() + ", variableAssignNode]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        // Dynamic cast to TimeLiteralNode and value check
        if (auto o = dynamic_cast<const TimeLiteralNode *>(other)) {
            return (value == o->value && type == o->type);
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
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
    Type type;

  public:
    /**
     * @brief Constructor for the binary expr node.
     * @param op Arithmetic or logical operator.
     * @param lhs Left-hand operand.
     * @param rhs Right-hand operand.
     */
    explicit BinaryExprNode(const std::string &op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)), type(Type(SupportedTypes::TYPE_VOID)) {}

    /**
     * @brief Devuelve el operador asociado al nodo.
     * @return Operador en formato string.
     */
    std::string getValue() const override { return op; }

    /**
     * @brief Returns the left hand side operator.
     * @return Returns a pointer to the left hand side node.
     */
    ASTNode *getLeft() const { return left.get(); }

    /**
     * @brief Returns the right hand side operator.
     * @return Returns a pointer to the right hand side node.
     */
    ASTNode *getRight() const { return right.get(); }

    /**
     * @brief Getter for the returnType.
     * @return Type of the result of this expression.
     */
    Type getType() const { return type; }

    /**
     * @brief Setter for the returnType.
     * @param t Type of this node.
     */
    void setType(Type t) { type = t; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        // Dynamic cast to BinaryExprNode and value check
        if (auto o = dynamic_cast<const BinaryExprNode *>(other)) {
            // Returns a comparison of their operator, lhs and rhs
            return op == o->op && left->equals(o->getLeft()) && right->equals(o->getRight());
        }

        return false;
    }

    /// @copydoc ASTNode::print
    std::string print() const override { return "\n[{" + op + "},binaryNode" + left->print() + right->print() + "]"; }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class VariableDec
 * @brief Represents a variable declaration in the AST.
 *
 * This class represents a variable declaration within the AST,
 * it has a value type and could be initialized with a value.
 *
 * @see ASTNode
 * @see CodeBlock
 */
class ReturnNode : public ASTNode {
    std::unique_ptr<ASTNode> stmt;

  public:
    /**
     * @brief Constructor for a return statement.
     * @param ret returned ASTNode.
     */
    explicit ReturnNode(std::unique_ptr<ASTNode> ret) : stmt(std::move(ret)){};

    /**
     * @brief Getter for stmt.
     * @return Statement returned by this node.
     */
    ASTNode *getStmt() const { return stmt.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return "RETURN: "; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        if (stmt != nullptr) {
            return "\n[RETURN,returnNode" + stmt.get()->print() + "]";
        }
        return "\n[RETURN void,returnNode]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const ReturnNode *>(other)) {
            // Returns the result of comparing all the attributes
            return stmt.get()->equals(o->getStmt());
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class VariableDec
 * @brief Represents a variable declaration in the AST.
 *
 * This class represents a variable declaration within the AST,
 * it has a value type and could be initialized with a value.
 *
 * @see ASTNode
 * @see CodeBlock
 */
class VariableDecNode : public ASTNode {
    Type type;
    std::string identifier;

  public:
    /**
     * @brief Constructor for a uninitialized VariableDec node.
     * @param t Type of the variable.
     * @param id Identifier of this variable.
     */
    explicit VariableDecNode(Type t, const std::string &id) : type(t), identifier(id){};

    /**
     * @brief Getter for type.
     * @return Type of this variable.
     */
    Type getType() const { return type; }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    std::string print() const override { return "\n[{" + typeToString(type) + " " + identifier + "},variableDecNode]"; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const VariableDecNode *>(other)) {
            // Returns the result of comparing all the attributes
            return type == o->type && identifier == o->identifier;
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class VariableAssign
 * @brief Represents a variable assignation in the AST.
 *
 * This class represents a variable assignation within the AST,
 * it has a identifier and a new value assigned to this identifier.
 *
 * @see ASTNode
 * @see CodeBlock
 */
class VariableAssignNode : public ASTNode {
    Type type;
    std::string identifier;
    std::unique_ptr<ASTNode> assign;

  public:
    /**
     * @brief Constructor for VariableAssign node.
     * @param t Type of the variable being assigned.
     * @param id Identifier of this variable.
     * @param val Value assigned to the variable.
     */
    explicit VariableAssignNode(Type t, const std::string &id, std::unique_ptr<ASTNode> val)
        : type(t), identifier(id), assign(std::move(val)){};

    /**
     * @brief Getter for type.
     * @return Type of the variable being assigned.
     */
    Type getType() const { return type; }

    /**
     * @brief Getter for assign expresion.
     * @return Statement with the value assigned to the variable.
     */
    ASTNode *getAssign() const { return assign.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        return "\n[{" + typeToString(type) + " " + identifier + "},variableAssignNode " + assign->print() + "]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const VariableAssignNode *>(other)) {
            // Returns the result of comparing all the attributes
            return identifier == o->identifier && assign.get()->equals(o->assign.get()) && type == o->type;
        }

        return false;
    }
    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class VariableRefNode
 * @brief Represents a variable assignation in the AST.
 *
 * This class represents a variable assignation within the AST,
 * it has a identifier and a new value assigned to this identifier.
 *
 * @see ASTNode
 */
class VariableRefNode : public ASTNode {
    std::string identifier;
    bool ref = false; /// Marks this variable as a reference (a pointer to the variable)

  public:
    /**
     * @brief Constructor for VariableRef node.
     * @param id Name of this reference.
     */
    explicit VariableRefNode(const std::string &id, bool b = false) : identifier(id), ref(b){};

    /**
     * @brief Returns true if this variable ref is a pointer, false otherwise.
     * @return `true` if this variable is a reference, `false` otherwise.
     */
    bool isRef() const { return ref; }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        if (ref) {
            return "\n[{ptr->" + identifier + "},varRefNode]";
        }

        return "\n[{" + identifier + "},varRefNode]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const VariableRefNode *>(other)) {
            // Returns the result of comparing all the attributes
            return identifier == o->identifier;
        }

        return false;
    }
    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class FunctionDecNode
 * @brief Represents a function declaration in the AST.
 *
 * This class represents a function declaration within the AST,
 * it has a identifier, return type and parameters.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see VariableDecNode
 */
class FunctionDecNode : public ASTNode {
    std::string identifier;
    std::vector<Type> paramList;
    Type type;

  public:
    /**
     * @brief Constructor for the FunctionDecNode.
     * @param id Name of the function.
     * @param params Params of the function.
     * @param t Return type of the function.
     */
    explicit FunctionDecNode(const std::string &id, std::vector<Type> &params, Type t)
        : identifier(id), paramList(params), type(t){};

    /**
     * @brief Getter for type.
     * @return Function return type.
     */
    Type getType() const { return type; }

    /**
     * @brief Getter for the params size.
     * @return Amount of params of this function.
     */
    int getParamsCount() const { return paramList.size(); }

    /**
     * @brief Getter for a single params.
     * @param i Index of the parameter in the paramList.
     * @return Type of the parameter with index i.
     */
    Type getParam(int i) const { return paramList[i]; }

    /**
     * @brief Getter for params.
     * @return Vector with the parameters.
     */
    std::vector<Type> getParams() const { return paramList; }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        std::string params;
        for (int i = 0; i < paramList.size(); i++) {
            params.append(typeToString(getParam(i)));
        }

        return "\n[{" + identifier + params + "},functionDecNode]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const FunctionDecNode *>(other)) {
            // Returns the result of comparing all the attributes
            return identifier == o->identifier && type == o->type;
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class FunctionDefNode
 * @brief Represents a function definition in the AST.
 *
 * This class represents a function definition within the AST,
 * this node provides a code block with additional statements.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see VariableDecNode
 */
class FunctionDefNode : public ASTNode {
    std::string identifier;
    std::vector<std::unique_ptr<ASTNode>> paramList;
    Type type;
    std::unique_ptr<CodeBlockNode> codeBlock;

  public:
    /**
     * @brief Constructor for the FunctionDefNode.
     * @param id Name of the function.
     * @param params Types of the params of this function.
     * @param t Return type of the function.
     * @param code Block of code executed at function call.
     */
    explicit FunctionDefNode(const std::string &id,
                             std::vector<std::unique_ptr<ASTNode>> &params,
                             Type t,
                             std::unique_ptr<CodeBlockNode> code)
        : identifier(id), paramList(std::move(params)), type(t), codeBlock(std::move(code)){};

    /**
     * @brief Getter for type.
     * @return Tyoe assiciated with this node.
     */
    Type getType() const { return type; }

    /**
     * @brief Getter for code block inside this function.
     * @return Statements defined in this function block of code.
     */
    CodeBlockNode *getCodeBlock() const { return codeBlock.get(); }

    /**
     * @brief Returns the ammount of parameters in this function.
     * @return Amount of parameters in this function.
     */
    int getParamsCount() const { return paramList.size(); }

    /**
     * @brief Returns the parameters with index i.
     * @return Single parameter.
     */
    ASTNode *getParam(int i) const { return paramList[i].get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        std::string params;
        for (int i = 0; i < paramList.size(); i++) {
            params.append(getParam(i)->print());
        }

        return "\n[" + identifier + ",functionCallNode" + params + codeBlock->print() + "]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const FunctionDefNode *>(other)) {
            // Returns the result of comparing all the attributes
            return identifier == o->identifier && type == o->type && codeBlock.get()->equals(o->codeBlock.get());
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class FunctionDecNode
 * @brief Represents a function declaration in the AST.
 *
 * This class represents a function declaration within the AST,
 * it has a return type, a CodeBlock and parameters.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see VariableDecNode
 */
class FunctionCallNode : public ASTNode {
    std::string identifier;
    std::vector<std::unique_ptr<ASTNode>> paramList;

  public:
    /**
     * @brief Constructor for the FunctionDecNode.
     * @param id Name of the function.
     * @param params Types of the params of this function.
     */
    explicit FunctionCallNode(const std::string &id, std::vector<std::unique_ptr<ASTNode>> params)
        : identifier(id), paramList(std::move(params)){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /**
     * @brief Returns the ammount of parameters in this block.
     * @return Amount of parameters in this function call.
     */
    int getParamsCount() const { return paramList.size(); }

    /**
     * @brief Returns the parameters with index i.
     * @return Single parameter.
     */
    ASTNode *getParam(int i) const { return paramList[i].get(); }

    /// @copydoc ASTNode::print
    std::string print() const override {
        std::string params;
        for (int i = 0; i < paramList.size(); i++) {
            params.append(getParam(i)->print());
        }

        return "\n[" + identifier + ",functionCallNode" + params + "]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const FunctionCallNode *>(other)) {
            // Returns the result of comparing all the attributes
            return identifier == o->identifier;
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class IfNode
 * @brief Represents a if statement in the AST.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see BinaryExprNode
 */
class IfNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<CodeBlockNode> codeBlock;
    std::unique_ptr<ASTNode> elseStmt;

  public:
    /**
     * @brief Constructor for the IfNode.
     * @param expression Expression to be evaluated.
     * @param block Block of code executed if the expression is true.
     * @param els Else statement (optional).
     */
    explicit IfNode(std::unique_ptr<ASTNode> expression,
                    std::unique_ptr<CodeBlockNode> block,
                    std::unique_ptr<ASTNode> els = nullptr)
        : expr(std::move(expression)), codeBlock(std::move(block)), elseStmt(std::move(els)){};

    /**
     * @brief Getter for the expr.
     * @return Condition statement.
     */
    ASTNode *getExpr() { return expr.get(); }

    /**
     * @brief Getter for the codeBlock.
     * @return Statements executed if the condition is block.
     */
    ASTNode *getCodeBlock() { return codeBlock.get(); }

    /**
     * @brief Getter for the elseStmt.
     * @return Else statement node.
     */
    ASTNode *getElseStmt() { return elseStmt.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return "IF"; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        if (elseStmt != nullptr)
            return "\n[IF,IfNode" + expr->print() + codeBlock->print() + elseStmt->print() + "]";

        return "\n[IF,IfNode" + expr->print() + codeBlock->print() + "]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const IfNode *>(other)) {
            // Returns the result of comparing all the attributes
            return codeBlock.get()->equals(o->codeBlock.get()) && expr.get()->equals(o->expr.get());
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class ElseNode
 * @brief Represents a else statement in the AST.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see IfNode
 */
class ElseNode : public ASTNode {
    std::unique_ptr<ASTNode> stmt;

  public:
    /**
     * @brief Constructor for the ElseNode.
     * @param stmt Block of code or other if statement.
     */
    explicit ElseNode(std::unique_ptr<ASTNode> elseStmt) : stmt(std::move(elseStmt)){};

    /**
     * @brief Getter for the stmt.
     * @return If node or statement block node.
     */
    ASTNode *getStmt() { return stmt.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return "ELSE"; }

    /// @copydoc ASTNode::print
    std::string print() const override { return "\n[ELSE,ElseNode" + stmt->print() + "]"; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const ElseNode *>(other)) {
            // Returns the result of comparing all the attributes
            return stmt.get()->equals(o->stmt.get());
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class ElseNode
 * @brief Represents a while loop statement in the AST.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see BinaryExprNode
 */
class WhileNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<CodeBlockNode> codeBlock;

  public:
    /**
     * @brief Constructor for the while statement node.
     * @param expression Condition for the loop execution.
     * @param block Block of code executed in loop.
     */
    explicit WhileNode(std::unique_ptr<ASTNode> expression, std::unique_ptr<CodeBlockNode> block)
        : expr(std::move(expression)), codeBlock(std::move(block)){};

    /**
     * @brief Getter for the expr.
     * @return Node with the conditional statement.
     */
    ASTNode *getExpr() { return expr.get(); }

    /**
     * @brief Getter for the code block.
     * @return Block of code executed in each iteration.
     */
    ASTNode *getCodeBlock() { return codeBlock.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return "WHILE"; }

    /// @copydoc ASTNode::print
    std::string print() const override { return "\n[WHILE ,LoopNode" + expr->print() + codeBlock->print() + "]"; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const WhileNode *>(other)) {
            // Returns the result of comparing all the attributes
            return codeBlock.get()->equals(o->codeBlock.get()) && expr.get()->equals(o->expr.get());
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class ForNode
 * @brief Represents a for loop statement in the AST.
 *
 * @see ASTNode
 * @see CodeBlock
 * @see BinaryExprNode
 */
class ForNode : public ASTNode {
    std::unique_ptr<ASTNode> def;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> assign;
    std::unique_ptr<CodeBlockNode> codeBlock;

  public:
    /**
     * @brief Constructor for the for statement node.
     * @param definition Definition of the conditional variable.
     * @param loopCondition Condition for the loop execution.
     * @param assignation Transformation for the conditional variable in each iteration.
     * @param block Block of code executed in loop.
     */
    explicit ForNode(std::unique_ptr<ASTNode> definition,
                     std::unique_ptr<ASTNode> loopCondition,
                     std::unique_ptr<ASTNode> assignation,
                     std::unique_ptr<CodeBlockNode> block)
        : def(std::move(definition)), condition(std::move(loopCondition)), assign(std::move(assignation)),
          codeBlock(std::move(block)){};

    /**
     * @brief Getter for the condition definition.
     * @return Node with the conditional variable definition.
     */
    ASTNode *getDef() { return def.get(); }

    /**
     * @brief Getter for the condition expr.
     * @return Node with the condition.
     */
    ASTNode *getCondition() { return condition.get(); }

    /**
     * @brief Getter for the condition assign.
     * @return Node with the assign data.
     */
    ASTNode *getAssign() { return assign.get(); }

    /**
     * @brief Getter for the code block.
     * @return Block of code executed in each iteration.
     */
    ASTNode *getCodeBlock() { return codeBlock.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return "FOR"; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        return "\n[FOR ,LoopNode" + def->print() + condition->print() + assign->print() + codeBlock->print() + "]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const ForNode *>(other)) {
            // Returns the result of comparing all the attributes
            return codeBlock.get()->equals(o->codeBlock.get()) && def.get()->equals(o->def.get()) &&
                   condition.get()->equals(o->condition.get()) && assign.get()->equals(o->assign.get());
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class LoopControlStatementNode
 * @brief Represents a loop control statement like break or continue.
 *
 * This class will represent a change in the execution order of a loop.
 *
 * @see ASTNode
 * @see WhileNode
 * @see ForNode
 */
class LoopControlStatementNode : public ASTNode {
    std::string id;

  public:
    /**
     * @brief Constructor for the loop control statement node.
     * @param id Keyword of the statement, could be continue or break.
     */
    explicit LoopControlStatementNode(std::string identifier) : id(identifier){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return id; }

    /// @copydoc ASTNode::print
    std::string print() const override { return "\n[" + getValue() + ", returnNode]"; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const LoopControlStatementNode *>(other)) {
            // Returns the result of comparing all the attributes
            return id == o->id;
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class LoopControlStatementNode
 * @brief Represents a event statement definition.
 *
 * @see ASTNode
 * @see TimeLiteralNode
 */
class EventNode : public ASTNode {
    std::string id;
    TimeCommand command;
    int limit;
    std::vector<std::unique_ptr<ASTNode>> paramList;
    std::unique_ptr<ASTNode> timeStmt;
    std::unique_ptr<CodeBlockNode> codeBlock;

  public:
    /**
     * @brief Constructor for the event node.
     * @param id identifier of the event.
     * @param params parameters of this event.
     * @param timeCommand activation mechanism of this event node.
     * @param activationTime TimeLiteral / VariableRef with the time of the event.
     * @param codeBlock code executed in this event block.
     */
    explicit EventNode(std::string identifier,
                       std::vector<std::unique_ptr<ASTNode>> &params,
                       TimeCommand timeCommand,
                       std::unique_ptr<ASTNode> time,
                       std::unique_ptr<CodeBlockNode> block,
                       int execLimit = 0)
        : id(identifier), paramList(std::move(params)), command(timeCommand), timeStmt(std::move(time)),
          codeBlock(std::move(block)), limit(execLimit){};

    /**
     * @brief Getter for the code block.
     * @return Code block stored in this node.
     */
    ASTNode *getCodeBlock() { return codeBlock.get(); }

    /**
     * @brief Getter for the time statement.
     * @return Node with the time statement data.
     */
    ASTNode *getTimeStmt() { return timeStmt.get(); }

    /**
     * @brief Getter for the time command.
     * @return TimeCommand keyword.
     */
    TimeCommand getTimeCommand() { return command; }

    /**
     * @brief Returns the ammount of parameters in this event.
     * @return Amount of parameters in this event definition.
     */
    int getParamsCount() const { return paramList.size(); }

    /**
     * @brief Returns the parameters with index i.
     * @return Statement with parameter index i.
     */
    ASTNode *getParam(int i) const { return paramList[i].get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return id; }

    /// @copydoc ASTNode::print
    std::string print() const override {
        std::string params;
        for (int i = 0; i < paramList.size(); i++) {
            params.append(getParam(i)->print());
        }

        return "\n[" + id + " " + timeCommandToString(command) + ", params: " + params + "," + timeStmt->print() +
               codeBlock->print() + "]";
    }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const EventNode *>(other)) {
            // Returns the result of comparing all the attributes
            return id == o->id && timeStmt->equals(o->timeStmt.get()) && codeBlock->equals(o->codeBlock.get()) &&
                   command == o->command && paramList == o->paramList;
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};

/**
 * @class ExitNode
 * @brief Represents a control statement capable of stoping the execution of an event.
 *
 * @see ASTNode
 * @see EventNode
 */
class ExitNode : public ASTNode {
    std::string id;

  public:
    /**
     * @brief Constructor for the exit node.
     * @param id identifier of the event to exit.
     */
    explicit ExitNode(std::string identifier) : id(identifier){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return id; }

    /// @copydoc ASTNode::print
    std::string print() const override { return "\n[EXIT" + id + ",returnNode]"; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const ExitNode *>(other)) {
            // Returns the result of comparing all the attributes
            return id == o->id;
        }

        return false;
    }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};
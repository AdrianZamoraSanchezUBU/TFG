/**
 * @file AST.h
 * @brief Contains the definition of the Abstract Syntax Tree (AST).
 *
 * @author Adrián Zamora Sánchez
 */

#pragma once
#include "SupportedTypes.h"
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
     */
    virtual void print() const = 0;
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
     */
    int getStmtCount() const { return statements.size(); }

    /**
     * @brief Returns the statement with index i.
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
    void print() const override { std::cout << "CODE BLOCK" << std::endl; }

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
    SupportedTypes type;

  public:
    /**
     * @brief Constructor for the literal node.
     * @param val Value associated with the node.
     */
    explicit LiteralNode(std::variant<int, float, char, std::string, bool> val, SupportedTypes t)
        : value(std::move(val)), type(t) {}

    /**
     * @brief Returns the value as std::variant.
     */
    const std::variant<int, float, char, std::string, bool> &getVariantValue() const { return value; }

    /**
     * @brief Getter for type.
     */
    SupportedTypes getType() const { return type; }

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
    void print() const override { std::cout << "LITERAL NODE" << getValue() << std::endl; }

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
    SupportedTypes type;

  public:
    /**
     * @brief Constructor for the binary expr node.
     * @param op Arithmetic or logical operator.
     * @param lhs Left-hand operand.
     * @param rhs Right-hand operand.
     */
    explicit BinaryExprNode(const std::string &op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)), type(SupportedTypes::TYPE_VOID) {}

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
     * @brief Getter for the returnType;
     */
    SupportedTypes getType() const { return type; }

    /**
     * @brief Setter for the returnType;
     */
    void setType(SupportedTypes t) { type = t; }

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
    void print() const override { std::cout << "BINARY EXPR NODE" << getValue() << std::endl; }

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
    SupportedTypes type = SupportedTypes::TYPE_VOID;
    std::unique_ptr<ASTNode> stmt;

  public:
    /**
     * @brief Constructor for a return statement.
     * @param ret returned ASTNode.
     */
    explicit ReturnNode(std::unique_ptr<ASTNode> ret) : stmt(std::move(ret)){};

    /**
     * @brief Constructor for a return statement node without parameters (return;).
     */
    explicit ReturnNode(){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return typeToString(type); }

    /**
     * @brief Getter for type.
     */
    SupportedTypes getType() const { return type; }

    /// @copydoc ASTNode::print
    void print() const override { std::cout << "RETURN STMT NODE" << getValue() << std::endl; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override {
        if (auto o = dynamic_cast<const ReturnNode *>(other)) {
            // Returns the result of comparing all the attributes
            return type == o->type && stmt.get()->equals(o);
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
    SupportedTypes type;
    std::string identifier;

  public:
    /**
     * @brief Constructor for a uninitialized VariableDec node.
     * @param t Type of the variable.
     * @param id Identifier of this variable.
     */
    explicit VariableDecNode(SupportedTypes t, const std::string &id) : type(t), identifier(id){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /**
     * @brief Getter for type.
     */
    SupportedTypes getType() const { return type; }

    /// @copydoc ASTNode::print
    void print() const override { std::cout << "VARIABLE DECLARATION NODE" << getValue() << std::endl; }

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
    SupportedTypes type;
    std::string identifier;
    std::unique_ptr<ASTNode> assign;

  public:
    /**
     * @brief Constructor for VariableAssign node.
     * @param id Identifier of this variable.
     * @param val Value assigned to the variable.
     */
    explicit VariableAssignNode(SupportedTypes t, const std::string &id, std::unique_ptr<ASTNode> val)
        : type(t), identifier(id), assign(std::move(val)){};

    /**
     * @brief Getter for type.
     */
    SupportedTypes getType() const { return type; }

    /**
     * @brief Getter for assign expresion.
     */
    ASTNode *getAssign() const { return assign.get(); }

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    void print() const override { std::cout << "VARIABLE ASSIGN NODE" << getValue() << std::endl; }

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
 * @see CodeBlock
 */
class VariableRefNode : public ASTNode {
    std::string identifier;

  public:
    /**
     * @brief Constructor for VariableAssign node.
     * @param id Identifier of this variable.
     * @param val Value assigned to the variable.
     */
    explicit VariableRefNode(const std::string &id) : identifier(id){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /// @copydoc ASTNode::print
    void print() const override { std::cout << "VARIABLE ASSIGN NODE" << getValue() << std::endl; }

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
 * it has a return type, a CodeBlock and parameters.
 *
 * @see ASTNode
 * @see CodeBlock
 */
class FunctionDecNode : public ASTNode {
    std::string identifier;
    std::vector<VariableDecNode> paramList;
    std::unique_ptr<CodeBlockNode> codeBlock;

  public:
    /**
     * @brief Constructor for the FunctionDecNode.
     * @param id Name os the function.
     */
    explicit FunctionDecNode(const std::string &id,
                             std::vector<VariableDecNode> &params,
                             std::unique_ptr<CodeBlockNode> code)
        : identifier(id), paramList(std::move(params)), codeBlock(std::move(code)){};

    /// @copydoc ASTNode::getValue
    std::string getValue() const override { return identifier; }

    /**
     * @brief  Indicates if the function is defined or only declared.
     * @return `true` if the function is defined, `false` otherwise.
     */
    bool isDefined() const {
        if (codeBlock) {
            return true;
        }

        return false;
    }

    /// @copydoc ASTNode::print
    void print() const override { std::cout << "FUNCTION DECLARATION NODE: " << getValue() << std::endl; }

    /// @copydoc ASTNode::equals
    bool equals(const ASTNode *other) const override { return false; }

    /// @copydoc ASTNode::accept(SemanticVisitor &)
    void *accept(SemanticVisitor &visitor) override;

    /// @copydoc ASTNode::accept(IRGenerator &visitor)
    llvm::Value *accept(IRGenerator &visitor) override;
};
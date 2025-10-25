#include "AST.h"

#include "IRGenerator.h"
#include "SemanticVisitor.h"

/* CodeBlockNode visitors */
void *CodeBlockNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value *CodeBlockNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

/* LiteralNode visitors */
void *LiteralNode::accept(SemanticVisitor &visitor) {}

llvm::Value *LiteralNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

/* BinaryExprNode visitors */
void *BinaryExprNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value *BinaryExprNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

/* VariableDecNode */
void *VariableDecNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *VariableDecNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};
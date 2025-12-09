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
void *LiteralNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value *LiteralNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

/* TimeLiteralNode visitors */
void *TimeLiteralNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value *TimeLiteralNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

/* BinaryExprNode visitors */
void *BinaryExprNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value *BinaryExprNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

/* UnaryOperationNode */
void *UnaryOperationNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *UnaryOperationNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* VariableDecNode */
void *VariableDecNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *VariableDecNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* VariableAssignNode */
void *VariableAssignNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *VariableAssignNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* VariableRefNode */
void *VariableRefNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *VariableRefNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* FunctionDefNode */
void *FunctionDefNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *FunctionDefNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* FunctionDecNode */
void *FunctionDecNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *FunctionDecNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* FunctionCallNode */
void *FunctionCallNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *FunctionCallNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* ReturnNode */
void *ReturnNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *ReturnNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* IfNode */
void *IfNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *IfNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* ElseNode */
void *ElseNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *ElseNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* WhileNode */
void *WhileNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *WhileNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* ForNode */
void *ForNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *ForNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* LoopControlStatementNode */
void *LoopControlStatementNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *LoopControlStatementNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* EventNode */
void *EventNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *EventNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};

/* ExitNode */
void *ExitNode::accept(SemanticVisitor &visitor) {
    return visitor.visit(*this);
};

llvm::Value *ExitNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
};
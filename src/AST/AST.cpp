#include "AST.h"

#include "IRGenerator.h"

llvm::Value *LiteralNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

llvm::Value *BinaryExprNode::accept(IRGenerator &visitor) {
    return visitor.visit(*this);
}

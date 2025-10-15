#include <string>
#include <memory>
#include <variant>

#include "CodegenContext.h"

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

	/**
	 * @brief Genera el LLVM IR asociado a este nodo.
	 * @return Un valor de LLVM (llvm::Value*).
	 */
	 virtual llvm::Value* codegen(CodegenContext& ctx) const = 0;
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

	/// @copydoc ASTNode::codegen
	llvm::Value* codegen(CodegenContext& ctx) const override {
		if(std::holds_alternative<int>(value)) {
			int v = std::get<int>(value);

			// INT: signed 32 bits integer
			return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(32, v, true));
		} 

		if(std::holds_alternative<float>(value)) {
			float v = std::get<float>(value);

			// FLOAT
			return llvm::ConstantFP::get(ctx.IRContext, llvm::APFloat(v));
		} 

		if(std::holds_alternative<char>(value)) {
			char v = std::get<char>(value);

			// CHAR: unsigned 8 bits integer
			return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(8, v, false));
		} 

		if(std::holds_alternative<std::string>(value)) {
			const std::string& v = std::get<std::string>(value);

			// STRING: pointer (*i8) to constant null-terminated IR format string
			return ctx.IRBuilder.CreateGlobalStringPtr(v, ".str");
		} 

		if(std::holds_alternative<bool>(value)) {
			bool v = std::get<bool>(value);

			// BOOL: unsigned 1 bit integer
			return llvm::ConstantInt::get(ctx.IRContext, llvm::APInt(1, v ? 1 : 0, false));
		}
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

    /// @copydoc ASTNode::codegen
   	llvm::Value* codegen(CodegenContext& ctx) const override {
		// left and right LLVM Values
		llvm::Value* L = left->codegen(ctx);
	    llvm::Value* R = right->codegen(ctx);

		// left and right LLVM Types
	    llvm::Type* LT = L->getType();
	    llvm::Type* RT = R->getType();

		/* 
		Type domination: float over int

		If one side of the operation is float and the other is int,
		the int will be cast into a float and then proceed with
		the operation.
		*/
		if (LT->isIntegerTy() && RT->isFloatingPointTy()) {
			L = ctx.IRBuilder.CreateSIToFP(L, RT, "inttofloat");
		}
		else if (LT->isFloatingPointTy() && RT->isIntegerTy()) {
			R = ctx.IRBuilder.CreateSIToFP(R, LT, "inttofloat");
		}
		
		/* Arithmetic operations */
		// FLOAT
		if (LT->isFloatingPointTy()) {
		    if (op == "+") return ctx.IRBuilder.CreateFAdd(L, R, "addtmp");
		    if (op == "-") return ctx.IRBuilder.CreateFSub(L, R, "subtmp");
		    if (op == "*") return ctx.IRBuilder.CreateFMul(L, R, "multmp");
		    if (op == "/") return ctx.IRBuilder.CreateFDiv(L, R, "divtmp");
		}
		// INT
		else if (LT-->isIntegerTy()) {
		    if (op == "+") return ctx.IRBuilder.CreateAdd(L, R, "addtmp");
		    if (op == "-") return ctx.IRBuilder.CreateSub(L, R, "subtmp");
		    if (op == "*") return ctx.IRBuilder.CreateMul(L, R, "multmp");
		    if (op == "/") return ctx.IRBuilder.CreateSDiv(L, R, "divtmp");
		}

		/* Logical operations */
		// FLOAT
		if (LT->isFloatingPointTy()) {
		    if (op == "==") return ctx.IRBuilder.CreateFCmpOEQ(L, R, "eqtmp");
		    if (op == "!=") return ctx.IRBuilder.CreateFCmpONE(L, R, "netmp");
		    if (op == "<")  return ctx.IRBuilder.CreateFCmpOLT(L, R, "lttmp");
		    if (op == ">")  return ctx.IRBuilder.CreateFCmpOGT(L, R, "gttmp");
		}
		// INT
		else if (LT-->isIntegerTy()) {
		    if (op == "==") return ctx.IRBuilder.CreateICmpEQ(L, R, "eqtmp");
		    if (op == "!=") return ctx.IRBuilder.CreateICmpNE(L, R, "netmp");
		    if (op == "<")  return ctx.IRBuilder.CreateICmpSLT(L, R, "lttmp");
		    if (op == ">")  return ctx.IRBuilder.CreateICmpSGT(L, R, "gttmp");
		}
		
		// STRING
		if (auto *ptrTyL = llvm::dyn_cast<llvm::PointerType>(L->getType())) {
			// C context
			llvm::LLVMContext &C = ctx.IRModule->getContext();
			llvm::Type *i8Ty = llvm::Type::getInt8Ty(C);
			llvm::Type *i8PtrTy = llvm::PointerType::getUnqual(i8Ty);

			// Function strcmp definition
			llvm::FunctionType *strcmpType = llvm::FunctionType::get(
				llvm::Type::getInt32Ty(C), // Return type
				{i8PtrTy, i8PtrTy}, // Params type
				false
			);

			// Getting the strcmp function
			llvm::FunctionCallee strcmpFunc = ctx.IRModule->getOrInsertFunction("strcmp", strcmpType);

			// Calling strcmp with L and R
			llvm::Value *strcmpResult = ctx.IRBuilder.CreateCall(strcmpFunc, {L, R});

			// Creates a boolean comparation for the result
			llvm::Value *isEqual = ctx.IRBuilder.CreateICmpEQ(
				strcmpResult,
				llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 0)
			);

			// Returns 0 if the strings are not equal and 1 if they are equal
			return ctx.IRBuilder.CreateSelect(
				isEqual,
				llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 1),
				llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 0)
			);
		}
   	}
};

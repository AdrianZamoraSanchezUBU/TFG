/**
 * @file Compiler.h
 * @brief Defines the main compiler pipeline and its core compilation phases.
 *
 * This file declares the `Compiler` class, which coordinates the entire
 * compilation process: lexical analysis, syntactical analysis, semantic analysis,
 * and LLVM IR generation. It acts as the high-level controller that connects
 * the ANTLR-generated lexer and parser, the AST builder, the semantic
 * analysis phase, and the LLVM IR generation.
 *
 * The class stores all intermediate representations such as the token stream,
 * abstract syntax tree (AST), symbol table, and LLVM context. It also manages
 * his own options (compiler flags).
 *
 * @see CompilerFlags
 * @see ASTBuilder
 * @see SymbolTable
 * @see IRGenerator
 * @see TLexer
 * @see TParser
 *
 * @author Adrián Zamora Sánchez
 */

#include "CompilerError.h"
// #include "spdlog/spdlog.h"

// FLAGS
#include "CompilerFlags.h"

// ANTLR
#include "LexerErrorListener.h"
#include "ParserErrorListener.h"
#include "TLexer.h"

// AST
#include "ASTBuilder.h"

// Semantic
#include "SemanticVisitor.h"

// LLVM
#include "IRGenerator.h"

#include "/usr/include/llvm-18/llvm/TargetParser/Host.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

class Compiler {
    CompilerFlags flags; ///< Flags

    /// Data structures
    std::unique_ptr<antlr4::ANTLRInputStream> inputStream;
    std::shared_ptr<antlr4::CommonTokenStream> tokenList = nullptr;
    std::unique_ptr<ASTNode> ast = nullptr;
    SymbolTable symTable;

    /// Workers
    std::unique_ptr<TLexer> lexer;
    std::unique_ptr<SemanticVisitor> analyser;
    std::unique_ptr<IRGenerator> IRgen;

    /// Error management
    std::vector<CompilerError> errorList;
    std::shared_ptr<ParserErrorListener> parserErrorListener;
    std::shared_ptr<LexerErrorListener> lexerErrorListener;

    std::ofstream texFile; /// File for AST visualization

  public:
    /**
     * @brief Compiler default constructor.
     * @param flagStruct Structure with the compiler flags data.
     */
    explicit Compiler(CompilerFlags flagsStruct) : flags(flagsStruct) {
        analyser = std::make_unique<SemanticVisitor>(symTable, errorList);
        IRgen = std::make_unique<IRGenerator>(symTable, errorList);
    };

    /// Lexical analysis phase of the compiler.
    void lex();

    /// Syntactic analysis phase of the compiler.
    void parse();

    /// Semantic analysis phase of the compiler.
    void analyze();

    /// IR generation phase of the compiler.
    void generateIR();

    /// Object code generation phase of the compiler.
    void generateObjectCode();

    /// Object code linkage and executable generation.
    void linkObjectFile();

    /// Getter for the error count.
    int getErrorCount() const { return errorList.size(); }

    /// Prints the error information.
    void printErrors();

    /**
     * @brief IRGenerator CodegenContext getter;
     * @return Structure with all the LLVM IR generation data.
     */
    CodegenContext &getIRContext() const { return IRgen.get()->getContext(); }

    /**
     * @brief AST getter.
     * @return Root node of the AST.
     */
    ASTNode *getAST() const { return ast.get(); }
};
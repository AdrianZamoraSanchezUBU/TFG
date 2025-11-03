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

    /// Lexer & Parser error listeners
    std::shared_ptr<ParserErrorListener> parserErrorListener;
    std::shared_ptr<LexerErrorListener> lexerErrorListener;

  public:
    /**
     * @brief Compiler default constructor.
     * @param flagStruct Structure with the compiler flags data.
     */
    explicit Compiler(CompilerFlags flagsStruct) : flags(flagsStruct) {
        analyser = std::make_unique<SemanticVisitor>(symTable);
        IRgen = std::make_unique<IRGenerator>(symTable);
    };

    /**
     * @brief Lexical analysis phase of the compiler.
     */
    void lex();

    /**
     * @brief Syntactic analysis phase of the compiler.
     */
    void parse();

    /**
     * @brief Semantic analysis phase of the compiler.
     */
    void analyze();

    /**
     * @brief IR generation phase of the compiler.
     */
    void generateIR();

    /**
     * @brief IRGenerator CodegenContext getter;
     */
    CodegenContext &getIRContext() const { return IRgen.get()->getContext(); }

    /**
     * @brief AST getter.
     */
    ASTNode *getAST() const { return ast.get(); }
};
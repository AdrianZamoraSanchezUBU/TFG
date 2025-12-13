#pragma once
#include <string>

/**
 * Enumeration of the compiler phases
 */
enum CompilerPhase { LEXER, PARSER, AST_BUILDER, SEMANTIC, IR_GEN, OPTIMIZATION, OBJECT_GEN, EXEC_GEN };

inline std::string phaseToString(CompilerPhase p) {
    switch (p) {
    case CompilerPhase::LEXER:
        return "Lexer phase";
    case CompilerPhase::PARSER:
        return "Parser phase";
    case CompilerPhase::AST_BUILDER:
        return "AST building phase";
    case CompilerPhase::SEMANTIC:
        return "Semantic analysis phase";
    case CompilerPhase::IR_GEN:
        return "LLVM IR generation phase";
    case CompilerPhase::OPTIMIZATION:
        return "Optimization phase";
    case CompilerPhase::OBJECT_GEN:
        return "Object code generation phase";
    case CompilerPhase::EXEC_GEN:
        return "Executable generation phase";
    default:
        break;
    }
}

/**
 * Structure that contains information about the location of a symbol in the source code.
 */
struct SourceLocation {
    int line;
    int column;

    SourceLocation() : line(-1), column(-1){};
    SourceLocation(int l, int c) : line(l), column(c){};
};

/**
 * Information of a compiler error.
 */
struct CompilerError {
    CompilerPhase phase;
    SourceLocation location;
    std::string offendingSymbol;
    std::string message;

    CompilerError(CompilerPhase p, SourceLocation loc, std::string sym, std::string msg)
        : phase(p), location(loc), offendingSymbol(std::move(sym)), message(std::move(msg)) {}
};

/**
 * @file LexerErrorListener.h
 * @brief Contains the definition of a custom error listener for the Lexer.
 *
 * @author Adrián Zamora Sánchez
 */

#include "antlr4-runtime.h"

/**
 * @brief Custom error listener for the Lexer.
 */
class LexerErrorListener : public antlr4::BaseErrorListener {
    std::vector<CompilerError> errors;

  public:
    void syntaxError(antlr4::Recognizer *,
                     antlr4::Token *token,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string &msg,
                     std::exception_ptr) override {

        errors.emplace_back(CompilerPhase::LEXER,
                            SourceLocation{static_cast<int>(line), static_cast<int>(charPositionInLine)},
                            token->getText(), msg);
    }

    /// @brief Returns true if a error was found, false in the other case.
    bool hasErrors() const { return !errors.empty(); }

    /// Getter for the error list
    const std::vector<CompilerError> &getErrors() const { return errors; }
};

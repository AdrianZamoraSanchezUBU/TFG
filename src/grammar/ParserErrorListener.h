/**
 * @file ParserErrorListener.h
 * @brief Contains the definition of a custom error listener for the Parser.
 * 
 * @author Adrián Zamora Sánchez
 */

#include "antlr4-runtime.h"

/**
 * @brief Custom error listener for the Parser.
 */
class ParserErrorListener : public antlr4::BaseErrorListener {
	bool errors = false;
public:
	void syntaxError(antlr4::Recognizer *recognizer,
					 antlr4::Token *offendingSymbol,
					 size_t line,
					 size_t charPositionInLine,
					 const std::string &msg,
                     std::exception_ptr e) override {
	// Changes error to true
	errors = true;

	// Shows error data
	std::cerr << "Syntax error in line "
			  << line << ":" << charPositionInLine
			  << " near " << (offendingSymbol ? offendingSymbol->getText() : "<EOF>")
			  << " -> " << msg << std::endl;
	}

	/// @brief Returns true if a error was found, false in the other case.
	bool hasErrors() const { return errors; }
};

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"
#include "AST.h"
#include "ASTBuilder.h"
#include "LexerErrorListener.h"
#include "ParserErrorListener.h"


std::string readFile(const std::string fileName){
	std::ifstream testFile(fileName);
	// Opens the file
    if (!testFile.is_open()) {
    	// Throw error if couldnt open the file
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    std::ostringstream buffer;
    buffer << testFile.rdbuf(); // Reads all the file content
    return buffer.str();
}


int main(int argc, char* argv[]){
	// Arguments check
	if(argc < 1){
		std::cerr << "A file name is needed" << std::endl;
	}

	// File reading
	std::string fileContent = readFile(argv[1]);

	// Lexical analysis
	antlr4::ANTLRInputStream input(fileContent);
	TLexer lexer(&input);

	// Modified lexer error listener
	lexer.removeErrorListeners();
    lexer.addErrorListener(new LexerErrorListener()); 

	antlr4::CommonTokenStream tokens(&lexer);
	tokens.fill();
	
	// TODO: remove debug
	/*
	for (auto token : tokens.getTokens()) {
	    std::cout << token->toString() << std::endl;
	}	
	*/
	
	// Parsing process
	TParser parser(&tokens);

	// Modified parser error listener
	parser.removeErrorListeners();
    parser.addErrorListener(new ParserErrorListener()); 

	TParser::ProgramContext* tree = parser.program();

	// TODO: remove debug
	// std::cout << tree->toStringTree(&parser) << std::endl;

	// AST build process
	ASTBuilder builder;

	try{
		auto ast = builder.visit(tree);
	}
	catch(const std::exception& e){
		std::cerr << "Error during AST build process: " << e.what() << '\n';
	}
    
	
	return 0;
}

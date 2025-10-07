#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"
#include "AST.h"
#include "ASTBuilder.h"

std::string readFile(std::string fileName){
	std::string line;
	std::string fileContent = "";

	std::ifstream testFile(fileName);

	// File check
	if (testFile.is_open()) {
		// Adding all the lines to a string 
	  	while (getline (testFile,line)) {
	    	fileContent += line + '\n';
	  	}

	  	testFile.close();
	} 
	else {
		std::cerr << "No se pudo abrir el fichero" << std::endl;
		return NULL;
	} 

	return fileContent;
}

int main(int argc, char* argv[]){
	// Arguments check
	if(argc < 1){
		std::cerr << "No se ha pasado un fichero objetivo" << std::endl;
	}

	// File reading
	std::string fileContent = readFile(argv[1]);

	// Lexical analysis
	antlr4::ANTLRInputStream input(fileContent);
	TLexer lexer(&input);

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

	TParser::ProgramContext* tree = parser.program();

	// TODO: remove debug
	// std::cout << tree->toStringTree(&parser) << std::endl;

	ASTBuilder builder;
    auto ast = builder.visit(tree);
	
	return 0;
}

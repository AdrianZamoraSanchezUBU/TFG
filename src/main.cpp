#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// ANTLR
#include "TLexer.h"
#include "LexerErrorListener.h"
#include "ParserErrorListener.h"

// AST
#include "ASTBuilder.h"

// LLVM
#include "IRGenerator.h"
#include <llvm/Support/raw_ostream.h>

/**
 * @brief Devuelve el texto contenido en un archivo.
 * 
 * Devuelve el texto contenido en un archivo o lanza un runtime_error 
 * en caso de no poder abrir el archivo para su lectura.
 *
 * @param fileName Nombre del fichero que se quiere leer.
 * @return String del contenido del fichero.
 * @throw std::runtime_error Si el fichero no existe.
 */
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

/**
 * @brief Punto de entrada del compilador.
 *
 * Esta función realiza el proceso completo de compilado.
 *
 * @param argc Número de argumentos.
 * @param argv Vector con los argumentos.
 * @return Código de salida del programa (0 si todo fue correcto).
 */
int main(int argc, char* argv[]){
	// Arguments check
	if(argc < 1){
		std::cerr << "A file name is needed" << std::endl;
	}

	// File reading
	std::string fileContent = readFile(argv[1]);

	/* Lexical analysis */
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
	
	/* Parsing process */
	TParser parser(&tokens);

	// Modified parser error listener
	parser.removeErrorListeners();
    parser.addErrorListener(new ParserErrorListener()); 

	TParser::ProgramContext* tree = parser.program();

	// TODO: remove debug
	// std::cout << tree->toStringTree(&parser) << std::endl;

	/* AST build process */
	ASTBuilder builder;
	std::vector<std::unique_ptr<ASTNode>> ast;

	try{
		ast = builder.visit(tree);
	}
	catch(const std::exception& e){
		std::cerr << "Error during AST build process: " << e.what() << '\n';
	}

	// Codegen
	IRGenerator IRgen;
	llvm::Value* result = ast[0]->accept(IRgen);

	// TODO: remove debug
	CodegenContext& ctx = IRgen.getContext();
	ctx.IRBuilder.CreateRet(result);
	ctx.IRModule->print(llvm::outs(), nullptr);
	
	return 0;
}

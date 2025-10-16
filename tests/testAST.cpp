#include <gtest/gtest.h>
#include "TLexer.h"
#include "TParser.h"
#include "AST.h"
#include "ASTBuilder.h"
#include "antlr4-runtime.h"
#include "ParserErrorListener.h"
#include <fstream>
#include <sstream>

/// copydoc readFile
std::string readFile(const std::string fileName){
    std::ifstream testFile(fileName);
    if (!testFile.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    std::ostringstream buffer;
    buffer << testFile.rdbuf(); // Reads the file
    return buffer.str();
}

/**
 * @brief Runs a test of the ASTBuilder for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return Root node of the generated AST.
 */
std::unique_ptr<ASTNode> runASTTest(const std::string& fileName){
    std::string fileContent = readFile(fileName);

	// Lexing process
    antlr4::ANTLRInputStream input(fileContent);
    TLexer lexer(&input);

    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

	// Parsing process
    TParser parser(&tokens);

    parser.removeErrorListeners();
    auto* errorListener = new ParserErrorListener();
   	parser.addErrorListener(errorListener);

   	TParser::ProgramContext* tree = parser.program();

	// AST build process
	ASTBuilder builder;
  
	try{
 		auto nodes = builder.visit(tree);
 		return std::move(nodes[0]);
 	}
 	catch(const std::exception& e){
 		std::cerr << "Error during AST build process: " << e.what() << '\n';
 	}

    return nullptr;
}

TEST(ASTTest, Expr){
        const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";

		// Result of the ASTBuilder
        auto result = runASTTest(fileName);

		/* Expected result */
        auto leftOperation = std::make_unique<BinaryExprNode>(
            "+",
            std::make_unique<LiteralNode>(3),
            std::make_unique<LiteralNode>(2)
        );
        
        auto rightOperation = std::make_unique<BinaryExprNode>(
            "+",
            std::make_unique<LiteralNode>(2),
            std::make_unique<LiteralNode>(1)
        );
        
        auto root = std::make_unique<BinaryExprNode>(
            "-",
            std::move(leftOperation),
            std::move(rightOperation)
        );

        EXPECT_TRUE(result->equals(root.get())) << "Unexpected AST result with: " << fileName;
}

/**
 * @brief Runs the tests associated with the ASTBuilder.
 * @see ASTNode
 * @see ASTBuilder
 */
int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

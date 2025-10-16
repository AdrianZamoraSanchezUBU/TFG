#include <gtest/gtest.h>
#include "TLexer.h"
#include "antlr4-runtime.h"
#include "LexerErrorListener.h"
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
 * @brief Runs a test of the lexer for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return `true` if the lexer accepted the code, `false` otherwise.
 */
bool runLexerTest(const std::string& fileName){
	std::string fileContent = readFile(fileName);
	
	antlr4::ANTLRInputStream input(fileContent);
	TLexer lexer(&input);

	lexer.removeErrorListeners();
    auto* errorListener = new LexerErrorListener();
    lexer.addErrorListener(errorListener);


	antlr4::CommonTokenStream tokens(&lexer);
	tokens.fill();

	// Verifies if an error is present 
    bool success = !errorListener->hasErrors();
	
	delete errorListener;
    return success;
}

TEST(LexerTest, BasicArithmeticExpr){
	const std::string fileName = std::string(TEST_FILES_DIR) + "basicArithmeticExpr.T";

	EXPECT_TRUE(runLexerTest(fileName)) << "Lexer fault at analizing the file: " << fileName;
}

TEST(LexerTest, BasicLogicalExpr){
	const std::string fileName = std::string(TEST_FILES_DIR) + "basicLogicalExpr.T";
	
	EXPECT_TRUE(runLexerTest(fileName)) << "Lexer fault at analizing the file:" << fileName;
}

TEST(LexerTest, ComplexExpr){
	const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";
	
	EXPECT_TRUE(runLexerTest(fileName)) << "Lexer fault at analizing the file:" << fileName;
}

/**
 * @brief Runs the tests associated with the lexer.
 */
int main(int argc, char **argv){
	::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

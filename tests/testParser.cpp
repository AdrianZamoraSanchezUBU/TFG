#include <gtest/gtest.h>
#include "TLexer.h"
#include "TParser.h"
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
 * @brief Runs a test of the parser for a specific case.
 *
 * @param fileName Name of the file with the code to test.
 * @return `true` if the parser accepted the code, `false` otherwise.
 */
bool runParserTest(const std::string& fileName){
    std::string fileContent = readFile(fileName);

    antlr4::ANTLRInputStream input(fileContent);
    TLexer lexer(&input);

	antlr4::CommonTokenStream tokens(&lexer);
	tokens.fill();

	TParser parser(&tokens);

    parser.removeErrorListeners();
    auto* errorListener = new ParserErrorListener();
    parser.addErrorListener(errorListener);

    TParser::ProgramContext* tree = parser.program();

    // Verifies if an error is present
    bool success = !errorListener->hasErrors();
    
    delete errorListener;
    return success;
}

TEST(ParserTest, BasicArithmeticExpr){
	const std::string fileName = std::string(TEST_FILES_DIR) + "basicArithmeticExpr.T";

	EXPECT_TRUE(runParserTest(fileName)) << "Parser fault at analizing the file: " << fileName;
}

TEST(ParserTest, BasicLogicalExpr){
	const std::string fileName = std::string(TEST_FILES_DIR) + "basicLogicalExpr.T";

	EXPECT_TRUE(runParserTest(fileName)) << "Parser fault at analizing the file:" << fileName;
}

TEST(ParserTest, ComplexExpr){
	const std::string fileName = std::string(TEST_FILES_DIR) + "complexExpr.T";

	EXPECT_TRUE(runParserTest(fileName)) << "Parser fault at analizing the file:" << fileName;
}

/**
 * @brief Runs the tests associated with the parser.
 */
int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

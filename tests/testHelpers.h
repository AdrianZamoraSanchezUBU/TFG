#include <gtest/gtest.h>

#include <fstream>
#include <regex>
#include <sstream>

#include "Compiler.h"
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

/**
 * @brief Run a test case for a file with a statement.
 *
 * @param fileName    Name of the filename with the test case
 * @param expectedAST Expected root for the generated AST.
 * @param expectedIR  Expected IR generated for this test case.
 *
 * @return `true` if the test was successful, `false` otherwise.
 */
bool test(const std::string &fileName, ASTNode *expectedAST, const std::string &expectedIR);

/**
 * @brief Run a test case for a file with a statement.
 *
 * @param fileName       Name of the filename with the test case
 * @param multipleRegex  Expected IR generated for this test case.
 *
 * @return `true` if the test was successful, `false` otherwise.
 */
bool test(const std::string &fileName, std::vector<std::string> multipleRegex);

/**
 * @brief Run a test case for a file with a statement.
 *
 * @param fileName       Name of the filename with the test case
 * @param expectedAST    Expected root for the generated AST.
 * @param multipleRegex  Expected IR generated for this test case.
 *
 * @return `true` if the test was successful, `false` otherwise.
 */
bool test(const std::string &fileName, ASTNode *expectedAST, std::vector<std::string> multipleRegex);
#include "testHelpers.h"

TEST(functionTest, functionDec) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionDec.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(declare i32 @foo)");

    test(fileName, regexpr);
}

TEST(functionTest, functionDef) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionDef.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(ret i32 %multmp)");

    test(fileName, regexpr);
}

TEST(functionTest, functionCall) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionCall.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(define i32 @foo)");
    regexpr.push_back(R"(i32 %x)");
    regexpr.push_back(R"(call i32 @foo)");

    test(fileName, regexpr);
}

TEST(functionTest, functionCallRef) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "functionCallRef.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(define i32 @foo)");
    regexpr.push_back(R"(ptr %x)");
    regexpr.push_back(R"(ptr %param_ptr)");

    test(fileName, regexpr);
}

/**
 * @brief Runs the tests associated with functions.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
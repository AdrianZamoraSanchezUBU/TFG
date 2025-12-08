#include "testHelpers.h"

TEST(eventTest, eventEvery) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "eventEvery.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back(R"(define void @test)");
    regexpr.push_back(R"(call void @registerEventData)");
    regexpr.push_back(R"(ptr @str)");
    regexpr.push_back(R"(float 2.)");
    regexpr.push_back(R"(ptr @test)");
    regexpr.push_back(R"(call void @scheduleEvent)");

    test(fileName, regexpr);
}

/**
 * @brief Runs the tests associated with expressions.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
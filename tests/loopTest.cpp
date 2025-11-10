#include "testHelpers.h"

TEST(loopTest, whileStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "while.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br label %condition");
    regexpr.push_back("condition:                                        ; preds = %loop, %entry");
    regexpr.push_back("br i1 %lttmp, label %loop, label %endLoop");
    regexpr.push_back("loop:                                             ; preds = %condition");
    regexpr.push_back("endLoop:                                          ; preds = %condition");

    test(fileName, regexpr);
}

TEST(loopTest, forStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "for.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br label %condition");
    regexpr.push_back("condition:                                        ; preds = %loop, %entry");
    regexpr.push_back("br i1 %lttmp, label %loop, label %endLoop");
    regexpr.push_back("loop:                                             ; preds = %condition");
    regexpr.push_back("store i32 %addtmp2, ptr %i_ptr, align 4");
    regexpr.push_back("endLoop:                                          ; preds = %condition");

    test(fileName, regexpr);
}

/**
 * @brief Runs the tests associated with loops.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
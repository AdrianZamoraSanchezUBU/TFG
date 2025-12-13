#include "testHelpers.h"

TEST(ifElseTest, ifStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "if.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 true, label %then, label %endif");

    test(fileName, regexpr);
}

TEST(ifElseTest, ifElseStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "else.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 false, label %then, label %else");
    regexpr.push_back("then:");
    regexpr.push_back("else:");
    regexpr.push_back("br label %endif");
    regexpr.push_back("endif:                                            ; preds = %else, %then");

    test(fileName, regexpr);
}

TEST(ifElseTest, nestedIf) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "nestedIf.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 %lttmp, label %then, label %endif");
    regexpr.push_back("br i1 %eqtmp, label %then1, label %endif2");
    regexpr.push_back("then:                                             ; preds = %entry");
    regexpr.push_back("endif:                                            ; preds = %entry");
    regexpr.push_back("then1:                                            ; preds = %then");
    regexpr.push_back("endif2:                                           ; preds = %then");

    test(fileName, regexpr);
}

TEST(loopTest, elseIfStatement) {
    const std::string fileName = std::string(TEST_FILES_DIR) + "elseif.T";

    /* Expected IR */
    std::vector<std::string> regexpr;
    regexpr.push_back("br i1 false, label %then, label %else");
    regexpr.push_back("then:                                             ; preds = %entry");
    regexpr.push_back("else:                                             ; preds = %entry");
    regexpr.push_back("endif:                                            ; preds = %endif3, %then");
    regexpr.push_back("then1:                                            ; preds = %else");
    regexpr.push_back("else2:                                            ; preds = %else");
    regexpr.push_back("endif3:                                           ; preds = %endif6, %then1");
    regexpr.push_back("then4:                                            ; preds = %else2");
    regexpr.push_back("else5:                                            ; preds = %else2");
    regexpr.push_back("endif6:                                           ; preds = %else5, %then4");

    test(fileName, regexpr);
}

/**
 * @brief Runs the tests associated with the if-else statement.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
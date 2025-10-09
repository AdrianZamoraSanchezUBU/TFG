cmake_minimum_required(VERSION 3.10)
project(TFG)

set(CMAKE_CXX_STANDARD 17)

# Including header files
include_directories(
    ${PROJECT_SOURCE_DIR}/src/grammar
    ${PROJECT_SOURCE_DIR}/src/AST
    /usr/local/include/antlr4-runtime
)
link_directories(/usr/local/lib)

# CPP executables
#add_executable(TCompiler
#    src/main.cpp
#    src/grammar/TLexer.cpp
#    src/grammar/TParser.cpp
#    src/AST/ASTBuilder.cpp
#)

add_library(compilerLib STATIC
    src/grammar/TLexer.cpp
    src/grammar/TParser.cpp
    src/AST/ASTBuilder.cpp
)
target_link_libraries(compilerLib PUBLIC antlr4-runtime)

add_executable(TCompiler src/main.cpp)

# Linking with antlr4-runtime
target_link_libraries(TCompiler PRIVATE compilerLib)

### Google test ###
add_definitions(-DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/tests/input/")

enable_testing()

add_executable(runLexerTests tests/testLexer.cpp)
target_link_libraries(runLexerTests PRIVATE compilerLib gtest gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(runLexerTests)

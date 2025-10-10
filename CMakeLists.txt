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
include(GoogleTest)
enable_testing()

# Inputs for tests
add_definitions(-DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/tests/input/")

# Tests souce
set(TEST_SOURCES
    tests/testLexer.cpp
    tests/testParser.cpp
)

# Build each test
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE compilerLib gtest gtest_main pthread)
    gtest_discover_tests(${test_name})
endforeach()

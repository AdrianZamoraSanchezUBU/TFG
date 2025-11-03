cmake_minimum_required(VERSION 3.10)
project(TFG)

set(CMAKE_CXX_STANDARD 17)

# Including header files
include_directories(
    ${PROJECT_SOURCE_DIR}/src/compiler
    ${PROJECT_SOURCE_DIR}/src/grammar
    ${PROJECT_SOURCE_DIR}/src/AST
    ${PROJECT_SOURCE_DIR}/src/semantic
    ${PROJECT_SOURCE_DIR}/src/LLVM
    ${PROJECT_SOURCE_DIR}/tests
    /usr/local/include/antlr4-runtime
)
link_directories(/usr/local/lib)

add_library(compilerLib STATIC
    src/compiler/Compiler.cpp
    src/compiler/CompilerFlags.cpp
    src/grammar/TLexer.cpp
    src/grammar/TParser.cpp
    src/AST/AST.cpp
    src/AST/ASTBuilder.cpp
    src/semantic/SemanticVisitor.cpp
    src/semantic/SymbolTable.cpp
    src/semantic/Scope.cpp
	src/LLVM/IRGenerator.cpp

    tests/testHelpers.cpp
)

# Libs LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Linking all the dependencies in one lib
target_link_libraries(compilerLib PUBLIC 
	antlr4-runtime
	LLVM
)

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
    tests/exprTest.cpp
    tests/variableTest.cpp
)

# Build each test
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE compilerLib gtest gtest_main pthread)
    gtest_discover_tests(${test_name})
endforeach()

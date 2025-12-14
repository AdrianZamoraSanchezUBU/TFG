cmake_minimum_required(VERSION 3.10)
project(TFG)

set(CMAKE_CXX_STANDARD 17)

# Libs
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(LLVM REQUIRED CONFIG)

set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")

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
)

# Compiler header files
target_include_directories(compilerLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/compiler
        ${PROJECT_SOURCE_DIR}/src/grammar
        ${PROJECT_SOURCE_DIR}/src/AST
        ${PROJECT_SOURCE_DIR}/src/semantic
        ${PROJECT_SOURCE_DIR}/src/LLVM
        ${PROJECT_SOURCE_DIR}/src/compat
        /usr/local/include/antlr4-runtime
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(compilerLib
    PRIVATE
        ${LLVM_DEFINITIONS}
        BUILD_DIR="${BUILD_DIR}" # Passing the build folder location to the main.cpp
)

# Linking all the dependencies in one lib
target_link_libraries(compilerLib 
    PUBLIC 
        antlr4-runtime
	    LLVM
        spdlog::spdlog
)

add_executable(TCompiler src/main.cpp) # Compiler executable main.cpp

# Runtime compilation
add_library(runtimeLib STATIC
    src/runtime/Event.cpp
    src/runtime/Runtime.cpp
    src/runtime/TLib.cpp
    src/runtime/main.cpp
)

# Runtime header files
target_include_directories(runtimeLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/runtime
)

# Runtime needed libs
target_link_libraries(runtimeLib
    PUBLIC
        spdlog::spdlog
        fmt::fmt
        pthread
)

# Linking with antlr4-runtime
target_link_libraries(TCompiler PRIVATE compilerLib)

### Google test ###
include(GoogleTest)
enable_testing()

# Inputs for tests
add_definitions(-DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/tests/input/")

find_library(GTEST_LIB gtest REQUIRED)
find_library(GTEST_MAIN_LIB gtest_main REQUIRED)

# Compilation of the tests helper functions
add_library(testSupport STATIC
    tests/testHelpers.cpp
)

# testHelper compilerLib dependency
target_link_libraries(testSupport
    PUBLIC
        compilerLib
)

# Headers for the tests
target_include_directories(testSupport
    PUBLIC
        ${PROJECT_SOURCE_DIR}/tests
)

# Tests sources
set(TEST_SOURCES
    tests/exprTest.cpp
    tests/variableTest.cpp
    tests/functionTest.cpp
    tests/ifElseTest.cpp
    tests/loopTest.cpp
    tests/eventTest.cpp
)

# Build each test
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(${test_name}
        ${test_src}
    )

    target_link_libraries(${test_name}
        PRIVATE
            testSupport
            ${GTEST_LIB}
            ${GTEST_MAIN_LIB}
            pthread
    )

    gtest_discover_tests(${test_name})
endforeach()
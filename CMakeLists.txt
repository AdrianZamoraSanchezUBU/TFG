cmake_minimum_required(VERSION 3.10)
project(TFG)

set(CMAKE_CXX_STANDARD 17)

# Including header files
include_directories(
    ${PROJECT_SOURCE_DIR}/src/grammar
    ${PROJECT_SOURCE_DIR}/src/AST
    /usr/local/include/antlr4-runtime
)

# Adding libs
link_directories(/usr/local/lib)

# CPP executables
add_executable(TCompiler
    src/main.cpp
    src/grammar/TLexer.cpp
    src/grammar/TParser.cpp
)

# Linking with antlr4-runtime
target_link_libraries(TCompiler antlr4-runtime)

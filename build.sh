#!/bin/bash
set -e # Stop in case of error

# Compiles the runtime and places the object file in build/
as ./src/runtime/runtime.s -o build/runtime.o

# Grammar cpp target generation
cd ./src/grammar/
java -Xmx500M -cp "$HOME/tools/antlr/antlr-4.13.1-complete.jar" org.antlr.v4.Tool -Dlanguage=Cpp TLexer.g4
java -Xmx500M -cp "$HOME/tools/antlr/antlr-4.13.1-complete.jar" org.antlr.v4.Tool -visitor -Dlanguage=Cpp TParser.g4
cd ../..

# Compiler build process
cd build 
cmake .. 
make -j$(nproc) # Using all cores

# Checks if param $1 is -test or --test
if [ "$1" = "-test" ] || [ "$1" = "--test" ]; 
then 
	# Runs all the tests in build/
	ctest --output-on-failure -T
fi

# Runs cppcheck as a simple code quality tool
cd ..
cppcheck --enable=all --inconclusive --std=c++17 --force --quiet --suppress=missingIncludeSystem src/ 2> cppcheck_out.txt
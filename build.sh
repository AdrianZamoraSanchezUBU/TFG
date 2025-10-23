#!/bin/bash
set -e # Stop in case of error

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
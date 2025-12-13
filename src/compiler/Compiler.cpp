#include "Compiler.h"

/**
 * @brief Adds the TickZ styles for the AST visualization.
 * @return string with the styles.
 */
std::string includeTikzStyles() {
    return R"(
\tikzset{
    programNode/.style={
        draw, 
        circle, 
        fill=purple!20!white,
        outer sep=2pt, 
        minimum size=2.5em,
        align=center
    },
    binaryNode/.style={
        draw, 
        diamond, 
        fill=yellow!25!white, 
        aspect=2, 
        minimum size=3em,
        align=center
    },
    literalNode/.style={
        draw, 
        ellipse, 
        fill=green!20!white, 
        minimum width=3.5em,
        minimum height=2.5em, 
        align=center
    },
    variableDecNode/.style={
        draw, 
        rectangle, 
        fill=cyan!15!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },
    varRefNode/.style={
        draw, 
        rectangle, 
        fill=green!10!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },variableAssignNode/.style={
        draw, 
        rectangle, 
        fill=violet!20!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },returnNode/.style={
        draw, 
        rectangle, 
        fill=violet, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },functionDecNode/.style={
        draw, 
        regular polygon, 
        regular polygon sides=5, 
        fill=teal!25!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },functionDefNode/.style={
        draw, 
        regular polygon, 
        regular polygon sides=6, 
        fill=teal!25!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },functionCallNode/.style={
        draw, 
        regular polygon,
        regular polygon sides=3, 
        fill=brown!20!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    },paramsNode/.style={
        draw, 
        rectangle, 
        fill=orange!10!white, 
        minimum size=2.5em,
        minimum width=2.5em,
        minimum height=2em, 
        align=center
    },IfNode/.style={
        draw, 
        rectangle, 
        fill=violet!10!white, 
        minimum size=2.7em,
        minimum width=3.2em,
        minimum height=2.2em, 
        align=center
    },ElseNode/.style={
        draw, 
        rectangle, 
        fill=violet!10!white, 
        minimum size=2.7em,
        minimum width=3.2em,
        minimum height=2.2em, 
        align=center
    },LoopNode/.style={
        draw, 
        regular polygon,
        regular polygon sides=3, 
        fill=red!20!white, 
        minimum size=3.5em,
        minimum width=4.5em,
        minimum height=2.8em, 
        align=center
    }
})";
}

/**
 * @brief Adds a header for the .tex output file when visualizing the AST.
 * @return string with the header.
 */
std::string includeTexHeader() {
    return R"(
\documentclass[border=5mm]{standalone}

\usepackage{bbding}
\usepackage{tikz,tikz-qtree,tikz-qtree-compat}
\usepackage{amssymb}
\usepackage{forest}

\usetikzlibrary{shapes}
\usetikzlibrary{positioning}
)" + includeTikzStyles() +
           R"(
\newcommand{\sep}{-.1mm}

\begin{document}

\begin{forest}
    for tree={
    align=center,
    parent anchor=south,
    child anchor=north,
    s sep=30pt,
    l sep=10pt,
    inner sep=1pt,
    }
[Program,programNode)";
}

void Compiler::lex() {
    // Reads the input file and generates the lexer
    std::string fileContent = readFile(flags.inputFile);
    inputStream = std::make_unique<antlr4::ANTLRInputStream>(fileContent);

    lexer = std::make_unique<TLexer>(inputStream.get());

    // Custom lexer error listener
    lexerErrorListener = std::make_shared<LexerErrorListener>();
    lexer->removeErrorListeners();
    lexer->addErrorListener(lexerErrorListener.get());

    // Checks for error
    if (lexerErrorListener->hasErrors()) {
        for (auto err : lexerErrorListener->getErrors()) {
            errorList.push_back(err);
        }
    }

    // Fill the token list
    tokenList = std::make_shared<antlr4::CommonTokenStream>(lexer.get());
    tokenList->fill();

    // Shows the tokens if the debug flag is set to true
    if (flags.debug) {
        // Printing the input text
        std::cout << "****** COMPILER INPUT ****** \n" << fileContent << "\n" << std::endl;

        // Printing tokens
        std::cout << "****** TOKEN LIST ******" << std::endl;
        for (auto token : tokenList->getTokens()) {
            std::cout << token->toString() << std::endl;
        }
        std::cout << std::endl;
    }
}

void Compiler::parse() {
    TParser parser(tokenList.get());

    // Custom lexer error listener
    parserErrorListener = std::make_shared<ParserErrorListener>();
    lexer->removeErrorListeners();
    lexer->addErrorListener(parserErrorListener.get());

    // Checks for error
    if (parserErrorListener->hasErrors()) {
        for (auto err : parserErrorListener->getErrors()) {
            errorList.push_back(err);
        }
    }

    TParser::ProgramContext *programCtx = parser.program();

    // AST generation process
    ASTBuilder builder(errorList);
    ast = builder.visit(programCtx);

    if (flags.visualizeAST || flags.debug) {
        // File clearing
        std::ofstream texFile("AST.tex", std::ios::trunc);
        if (!texFile.is_open()) {
            throw std::runtime_error("Couldn't open the AST.tex");
        }
        texFile.close();

        // File opening in append mode
        texFile.open("AST.tex", std::ios::app);
        if (!texFile.is_open()) {
            throw std::runtime_error("Couldn't open the AST.tex file for appending");
        }

        // Tex file header and styles
        texFile << includeTexHeader();

        texFile << ast->print() + "]" << std::endl;

        // Tex footer file
        texFile << R"(\end{forest})" << std::endl;
        texFile << R"(\end{document})" << std::endl;

        texFile.close();
    }

    // xelatex compilation and cleaning
    if (std::system("xelatex --version > /dev/null 2>&1") == 0 && (flags.visualizeAST || flags.debug)) {
        // Compiles the .tex file
        std::string texName = "AST";
        std::string command = "xelatex -interaction=nonstopmode " + texName + ".tex > /dev/null 2>&1";

        if (std::system(command.c_str()) == 0 && flags.debug) {
            std::cout << "****** AST VISUALIZATION GENERATED AT: ./AST.pdf ******\n" << std::endl;
        }

        // Clean the .log .aux and .tex files
        std::string cleanupCmd = "rm -f " + texName + ".log " + texName + ".aux " + texName + ".tex";
        std::system(cleanupCmd.c_str());
    }
}

void Compiler::analyze() {
    getAST()->accept(*analyzer);

    // Debug symbol table print
    if (flags.debug) {
        std::cout << "****** SYMBOL TABLE ******" << std::endl;
        analyzer->printSymbolTable();
        std::cout << std::endl;
    }
}

void Compiler::generateIR() {
    CodegenContext &ctx = IRgen.get()->getContext();
    getAST()->accept(*IRgen);

    // Debug IR print
    if (flags.debug) {
        std::cout << "****** GENERATED LLVM IR ******" << std::endl;
        ctx.IRModule->print(llvm::outs(), nullptr);
    }

    if (flags.generateIRFile != "") {
        std::error_code EC;
        llvm::raw_fd_ostream dest(flags.generateIRFile, EC, llvm::sys::fs::OF_None);
        ctx.IRModule->print(dest, nullptr);
    }
}

void Compiler::generateObjectCode() {
    // LLVM and CodegenContext set up
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    CodegenContext &ctx = IRgen.get()->getContext();

    // Getting the target tiple for this machine architecture
    std::string error;
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    // Set up for target options
    llvm::TargetOptions opt;
    auto relocModel = std::optional<llvm::Reloc::Model>();
    std::unique_ptr<llvm::TargetMachine> targetMachine(
        target->createTargetMachine(targetTriple, "generic", "", opt, relocModel));

    // Module data layout and target tiple configuration
    ctx.IRModule.get()->setDataLayout(targetMachine->createDataLayout());
    ctx.IRModule->setTargetTriple(targetTriple);

    // Object file destination
    std::error_code EC;
    llvm::raw_fd_ostream dest(std::string(BUILD_DIR) + "/" + flags.outputFile, EC, llvm::sys::fs::OF_None);

    // Object file emission
    llvm::legacy::PassManager emitPM;
    targetMachine->addPassesToEmitFile(emitPM, dest, nullptr, llvm::CodeGenFileType::ObjectFile);
    emitPM.run(*ctx.IRModule);
    dest.flush();

    if (flags.debug) {
        std::cout << "****** GENERATED OBJECT FILE ******" << std::endl;
    }
}

void Compiler::linkObjectFile() {
    std::string entryPoint = std::string(BUILD_DIR) + "/main.o ";
    std::string runtimeCpp = std::string(BUILD_DIR) + "/Runtime.o ";
    std::string eventCpp = std::string(BUILD_DIR) + "/Event.o ";
    std::string basicLib = std::string(BUILD_DIR) + "/TLib.o ";
    std::string programObjecFile = std::string(BUILD_DIR) + "/" + flags.outputFile;

    // Links the object file with the runtime to have a executable entry point and with a standard lib
    std::string command = "clang++ -no-pie " + entryPoint + basicLib + runtimeCpp + eventCpp + programObjecFile +
                          " -o program -pthread -lffi";
    std::system(command.c_str());
    std::system(("rm " + programObjecFile).c_str());

    if (flags.debug) {
        std::cout << "****** GENERATED EXECUTABLE ******" << std::endl;
    }
}

void Compiler::printErrors() {
    for (CompilerError err : errorList) {
        std::cout << "Error in " + phaseToString(err.phase) + " at: " + std::to_string(err.location.line) + ":" +
                         std::to_string(err.location.column) + " " + err.message
                  << std::endl;
    }
}

// Generated from src/grammar/TLexer.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  TLexer : public antlr4::Lexer {
public:
  enum {
    PLUS = 1, MINUS = 2, MUL = 3, DIV = 4, LPAREN = 5, RPAREN = 6, LBRACE = 7, 
    RBRACE = 8, IDENTIFIER = 9, NUMBER_LITERAL = 10, FLOAT_LITERAL = 11, 
    COMMNET = 12, NEWLINE = 13, WS = 14
  };

  explicit TLexer(antlr4::CharStream *input);

  ~TLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};


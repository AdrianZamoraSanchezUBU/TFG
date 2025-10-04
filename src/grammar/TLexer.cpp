
// Generated from src/grammar/TLexer.g4 by ANTLR 4.13.1


#include "TLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct TLexerStaticData final {
  TLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  TLexerStaticData(const TLexerStaticData&) = delete;
  TLexerStaticData(TLexerStaticData&&) = delete;
  TLexerStaticData& operator=(const TLexerStaticData&) = delete;
  TLexerStaticData& operator=(TLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag tlexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
TLexerStaticData *tlexerLexerStaticData = nullptr;

void tlexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (tlexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(tlexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<TLexerStaticData>(
    std::vector<std::string>{
      "PLUS", "MINUS", "MUL", "DIV", "LPAREN", "RPAREN", "LBRACE", "RBRACE", 
      "DIGIT", "LETTER", "IDENTIFIER", "NUMBER_LITERAL", "FLOAT_LITERAL", 
      "COMMNET", "NEWLINE", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'+'", "'-'", "'*'", "'/'", "'('", "')'", "'{'", "'}'"
    },
    std::vector<std::string>{
      "", "PLUS", "MINUS", "MUL", "DIV", "LPAREN", "RPAREN", "LBRACE", "RBRACE", 
      "IDENTIFIER", "NUMBER_LITERAL", "FLOAT_LITERAL", "COMMNET", "NEWLINE", 
      "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,14,111,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,1,0,1,0,1,1,1,1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,1,6,1,
  	6,1,7,1,7,1,8,1,8,1,9,1,9,1,10,4,10,55,8,10,11,10,12,10,56,1,10,5,10,
  	60,8,10,10,10,12,10,63,9,10,1,10,5,10,66,8,10,10,10,12,10,69,9,10,1,11,
  	4,11,72,8,11,11,11,12,11,73,1,12,4,12,77,8,12,11,12,12,12,78,1,12,1,12,
  	4,12,83,8,12,11,12,12,12,84,1,13,1,13,1,13,1,13,5,13,91,8,13,10,13,12,
  	13,94,9,13,1,13,1,13,1,14,4,14,99,8,14,11,14,12,14,100,1,14,1,14,1,15,
  	4,15,106,8,15,11,15,12,15,107,1,15,1,15,0,0,16,1,1,3,2,5,3,7,4,9,5,11,
  	6,13,7,15,8,17,0,19,0,21,9,23,10,25,11,27,12,29,13,31,14,1,0,4,1,0,48,
  	57,2,0,65,90,97,122,2,0,10,10,13,13,2,0,9,9,32,32,117,0,1,1,0,0,0,0,3,
  	1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,
  	0,15,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,
  	1,0,0,0,0,31,1,0,0,0,1,33,1,0,0,0,3,35,1,0,0,0,5,37,1,0,0,0,7,39,1,0,
  	0,0,9,41,1,0,0,0,11,43,1,0,0,0,13,45,1,0,0,0,15,47,1,0,0,0,17,49,1,0,
  	0,0,19,51,1,0,0,0,21,54,1,0,0,0,23,71,1,0,0,0,25,76,1,0,0,0,27,86,1,0,
  	0,0,29,98,1,0,0,0,31,105,1,0,0,0,33,34,5,43,0,0,34,2,1,0,0,0,35,36,5,
  	45,0,0,36,4,1,0,0,0,37,38,5,42,0,0,38,6,1,0,0,0,39,40,5,47,0,0,40,8,1,
  	0,0,0,41,42,5,40,0,0,42,10,1,0,0,0,43,44,5,41,0,0,44,12,1,0,0,0,45,46,
  	5,123,0,0,46,14,1,0,0,0,47,48,5,125,0,0,48,16,1,0,0,0,49,50,7,0,0,0,50,
  	18,1,0,0,0,51,52,7,1,0,0,52,20,1,0,0,0,53,55,3,19,9,0,54,53,1,0,0,0,55,
  	56,1,0,0,0,56,54,1,0,0,0,56,57,1,0,0,0,57,61,1,0,0,0,58,60,3,17,8,0,59,
  	58,1,0,0,0,60,63,1,0,0,0,61,59,1,0,0,0,61,62,1,0,0,0,62,67,1,0,0,0,63,
  	61,1,0,0,0,64,66,3,19,9,0,65,64,1,0,0,0,66,69,1,0,0,0,67,65,1,0,0,0,67,
  	68,1,0,0,0,68,22,1,0,0,0,69,67,1,0,0,0,70,72,3,17,8,0,71,70,1,0,0,0,72,
  	73,1,0,0,0,73,71,1,0,0,0,73,74,1,0,0,0,74,24,1,0,0,0,75,77,3,17,8,0,76,
  	75,1,0,0,0,77,78,1,0,0,0,78,76,1,0,0,0,78,79,1,0,0,0,79,80,1,0,0,0,80,
  	82,5,46,0,0,81,83,3,17,8,0,82,81,1,0,0,0,83,84,1,0,0,0,84,82,1,0,0,0,
  	84,85,1,0,0,0,85,26,1,0,0,0,86,87,5,47,0,0,87,88,5,47,0,0,88,92,1,0,0,
  	0,89,91,8,2,0,0,90,89,1,0,0,0,91,94,1,0,0,0,92,90,1,0,0,0,92,93,1,0,0,
  	0,93,95,1,0,0,0,94,92,1,0,0,0,95,96,6,13,0,0,96,28,1,0,0,0,97,99,7,2,
  	0,0,98,97,1,0,0,0,99,100,1,0,0,0,100,98,1,0,0,0,100,101,1,0,0,0,101,102,
  	1,0,0,0,102,103,6,14,0,0,103,30,1,0,0,0,104,106,7,3,0,0,105,104,1,0,0,
  	0,106,107,1,0,0,0,107,105,1,0,0,0,107,108,1,0,0,0,108,109,1,0,0,0,109,
  	110,6,15,0,0,110,32,1,0,0,0,10,0,56,61,67,73,78,84,92,100,107,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  tlexerLexerStaticData = staticData.release();
}

}

TLexer::TLexer(CharStream *input) : Lexer(input) {
  TLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *tlexerLexerStaticData->atn, tlexerLexerStaticData->decisionToDFA, tlexerLexerStaticData->sharedContextCache);
}

TLexer::~TLexer() {
  delete _interpreter;
}

std::string TLexer::getGrammarFileName() const {
  return "TLexer.g4";
}

const std::vector<std::string>& TLexer::getRuleNames() const {
  return tlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& TLexer::getChannelNames() const {
  return tlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& TLexer::getModeNames() const {
  return tlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& TLexer::getVocabulary() const {
  return tlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView TLexer::getSerializedATN() const {
  return tlexerLexerStaticData->serializedATN;
}

const atn::ATN& TLexer::getATN() const {
  return *tlexerLexerStaticData->atn;
}




void TLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  tlexerLexerInitialize();
#else
  ::antlr4::internal::call_once(tlexerLexerOnceFlag, tlexerLexerInitialize);
#endif
}

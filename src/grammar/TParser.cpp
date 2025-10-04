
// Generated from src/grammar/TParser.g4 by ANTLR 4.13.1


#include "TParserListener.h"
#include "TParserVisitor.h"

#include "TParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct TParserStaticData final {
  TParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  TParserStaticData(const TParserStaticData&) = delete;
  TParserStaticData(TParserStaticData&&) = delete;
  TParserStaticData& operator=(const TParserStaticData&) = delete;
  TParserStaticData& operator=(TParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag tparserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
TParserStaticData *tparserParserStaticData = nullptr;

void tparserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (tparserParserStaticData != nullptr) {
    return;
  }
#else
  assert(tparserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<TParserStaticData>(
    std::vector<std::string>{
      "program", "block", "stmt", "expr", "operand", "literal"
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
  	4,1,14,55,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,1,0,5,0,14,
  	8,0,10,0,12,0,17,9,0,1,1,1,1,5,1,21,8,1,10,1,12,1,24,9,1,1,1,1,1,1,2,
  	1,2,1,3,1,3,1,3,1,3,1,3,1,3,3,3,36,8,3,1,3,1,3,1,3,1,3,1,3,1,3,5,3,44,
  	8,3,10,3,12,3,47,9,3,1,4,1,4,3,4,51,8,4,1,5,1,5,1,5,0,1,6,6,0,2,4,6,8,
  	10,0,3,1,0,3,4,1,0,1,2,1,0,10,11,54,0,15,1,0,0,0,2,18,1,0,0,0,4,27,1,
  	0,0,0,6,35,1,0,0,0,8,50,1,0,0,0,10,52,1,0,0,0,12,14,3,4,2,0,13,12,1,0,
  	0,0,14,17,1,0,0,0,15,13,1,0,0,0,15,16,1,0,0,0,16,1,1,0,0,0,17,15,1,0,
  	0,0,18,22,5,7,0,0,19,21,3,4,2,0,20,19,1,0,0,0,21,24,1,0,0,0,22,20,1,0,
  	0,0,22,23,1,0,0,0,23,25,1,0,0,0,24,22,1,0,0,0,25,26,5,8,0,0,26,3,1,0,
  	0,0,27,28,3,2,1,0,28,5,1,0,0,0,29,30,6,3,-1,0,30,36,3,8,4,0,31,32,5,5,
  	0,0,32,33,3,6,3,0,33,34,5,6,0,0,34,36,1,0,0,0,35,29,1,0,0,0,35,31,1,0,
  	0,0,36,45,1,0,0,0,37,38,10,4,0,0,38,39,7,0,0,0,39,44,3,6,3,5,40,41,10,
  	3,0,0,41,42,7,1,0,0,42,44,3,6,3,4,43,37,1,0,0,0,43,40,1,0,0,0,44,47,1,
  	0,0,0,45,43,1,0,0,0,45,46,1,0,0,0,46,7,1,0,0,0,47,45,1,0,0,0,48,51,3,
  	10,5,0,49,51,5,9,0,0,50,48,1,0,0,0,50,49,1,0,0,0,51,9,1,0,0,0,52,53,7,
  	2,0,0,53,11,1,0,0,0,6,15,22,35,43,45,50
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  tparserParserStaticData = staticData.release();
}

}

TParser::TParser(TokenStream *input) : TParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

TParser::TParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  TParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *tparserParserStaticData->atn, tparserParserStaticData->decisionToDFA, tparserParserStaticData->sharedContextCache, options);
}

TParser::~TParser() {
  delete _interpreter;
}

const atn::ATN& TParser::getATN() const {
  return *tparserParserStaticData->atn;
}

std::string TParser::getGrammarFileName() const {
  return "TParser.g4";
}

const std::vector<std::string>& TParser::getRuleNames() const {
  return tparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& TParser::getVocabulary() const {
  return tparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView TParser::getSerializedATN() const {
  return tparserParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

TParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<TParser::StmtContext *> TParser::ProgramContext::stmt() {
  return getRuleContexts<TParser::StmtContext>();
}

TParser::StmtContext* TParser::ProgramContext::stmt(size_t i) {
  return getRuleContext<TParser::StmtContext>(i);
}


size_t TParser::ProgramContext::getRuleIndex() const {
  return TParser::RuleProgram;
}

void TParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void TParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


std::any TParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TParserVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

TParser::ProgramContext* TParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, TParser::RuleProgram);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(15);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == TParser::LBRACE) {
      setState(12);
      stmt();
      setState(17);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

TParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* TParser::BlockContext::LBRACE() {
  return getToken(TParser::LBRACE, 0);
}

tree::TerminalNode* TParser::BlockContext::RBRACE() {
  return getToken(TParser::RBRACE, 0);
}

std::vector<TParser::StmtContext *> TParser::BlockContext::stmt() {
  return getRuleContexts<TParser::StmtContext>();
}

TParser::StmtContext* TParser::BlockContext::stmt(size_t i) {
  return getRuleContext<TParser::StmtContext>(i);
}


size_t TParser::BlockContext::getRuleIndex() const {
  return TParser::RuleBlock;
}

void TParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void TParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}


std::any TParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TParserVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

TParser::BlockContext* TParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 2, TParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(18);
    match(TParser::LBRACE);
    setState(22);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == TParser::LBRACE) {
      setState(19);
      stmt();
      setState(24);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(25);
    match(TParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

TParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

TParser::BlockContext* TParser::StmtContext::block() {
  return getRuleContext<TParser::BlockContext>(0);
}


size_t TParser::StmtContext::getRuleIndex() const {
  return TParser::RuleStmt;
}

void TParser::StmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt(this);
}

void TParser::StmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt(this);
}


std::any TParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TParserVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

TParser::StmtContext* TParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 4, TParser::RuleStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(27);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

TParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

TParser::OperandContext* TParser::ExprContext::operand() {
  return getRuleContext<TParser::OperandContext>(0);
}

tree::TerminalNode* TParser::ExprContext::LPAREN() {
  return getToken(TParser::LPAREN, 0);
}

std::vector<TParser::ExprContext *> TParser::ExprContext::expr() {
  return getRuleContexts<TParser::ExprContext>();
}

TParser::ExprContext* TParser::ExprContext::expr(size_t i) {
  return getRuleContext<TParser::ExprContext>(i);
}

tree::TerminalNode* TParser::ExprContext::RPAREN() {
  return getToken(TParser::RPAREN, 0);
}

tree::TerminalNode* TParser::ExprContext::MUL() {
  return getToken(TParser::MUL, 0);
}

tree::TerminalNode* TParser::ExprContext::DIV() {
  return getToken(TParser::DIV, 0);
}

tree::TerminalNode* TParser::ExprContext::PLUS() {
  return getToken(TParser::PLUS, 0);
}

tree::TerminalNode* TParser::ExprContext::MINUS() {
  return getToken(TParser::MINUS, 0);
}


size_t TParser::ExprContext::getRuleIndex() const {
  return TParser::RuleExpr;
}

void TParser::ExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpr(this);
}

void TParser::ExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpr(this);
}


std::any TParser::ExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TParserVisitor*>(visitor))
    return parserVisitor->visitExpr(this);
  else
    return visitor->visitChildren(this);
}


TParser::ExprContext* TParser::expr() {
   return expr(0);
}

TParser::ExprContext* TParser::expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  TParser::ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, parentState);
  TParser::ExprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 6;
  enterRecursionRule(_localctx, 6, TParser::RuleExpr, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(35);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case TParser::IDENTIFIER:
      case TParser::NUMBER_LITERAL:
      case TParser::FLOAT_LITERAL: {
        setState(30);
        operand();
        break;
      }

      case TParser::LPAREN: {
        setState(31);
        match(TParser::LPAREN);
        setState(32);
        expr(0);
        setState(33);
        match(TParser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(45);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(43);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExprContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpr);
          setState(37);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(38);
          antlrcpp::downCast<ExprContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == TParser::MUL

          || _la == TParser::DIV)) {
            antlrcpp::downCast<ExprContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(39);
          expr(5);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExprContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpr);
          setState(40);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(41);
          antlrcpp::downCast<ExprContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == TParser::PLUS

          || _la == TParser::MINUS)) {
            antlrcpp::downCast<ExprContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(42);
          expr(4);
          break;
        }

        default:
          break;
        } 
      }
      setState(47);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- OperandContext ------------------------------------------------------------------

TParser::OperandContext::OperandContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

TParser::LiteralContext* TParser::OperandContext::literal() {
  return getRuleContext<TParser::LiteralContext>(0);
}

tree::TerminalNode* TParser::OperandContext::IDENTIFIER() {
  return getToken(TParser::IDENTIFIER, 0);
}


size_t TParser::OperandContext::getRuleIndex() const {
  return TParser::RuleOperand;
}

void TParser::OperandContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperand(this);
}

void TParser::OperandContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperand(this);
}


std::any TParser::OperandContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TParserVisitor*>(visitor))
    return parserVisitor->visitOperand(this);
  else
    return visitor->visitChildren(this);
}

TParser::OperandContext* TParser::operand() {
  OperandContext *_localctx = _tracker.createInstance<OperandContext>(_ctx, getState());
  enterRule(_localctx, 8, TParser::RuleOperand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(50);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case TParser::NUMBER_LITERAL:
      case TParser::FLOAT_LITERAL: {
        enterOuterAlt(_localctx, 1);
        setState(48);
        literal();
        break;
      }

      case TParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(49);
        match(TParser::IDENTIFIER);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LiteralContext ------------------------------------------------------------------

TParser::LiteralContext::LiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* TParser::LiteralContext::NUMBER_LITERAL() {
  return getToken(TParser::NUMBER_LITERAL, 0);
}

tree::TerminalNode* TParser::LiteralContext::FLOAT_LITERAL() {
  return getToken(TParser::FLOAT_LITERAL, 0);
}


size_t TParser::LiteralContext::getRuleIndex() const {
  return TParser::RuleLiteral;
}

void TParser::LiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void TParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}


std::any TParser::LiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TParserVisitor*>(visitor))
    return parserVisitor->visitLiteral(this);
  else
    return visitor->visitChildren(this);
}

TParser::LiteralContext* TParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 10, TParser::RuleLiteral);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(52);
    _la = _input->LA(1);
    if (!(_la == TParser::NUMBER_LITERAL

    || _la == TParser::FLOAT_LITERAL)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool TParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 3: return exprSempred(antlrcpp::downCast<ExprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool TParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 4);
    case 1: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

void TParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  tparserParserInitialize();
#else
  ::antlr4::internal::call_once(tparserParserOnceFlag, tparserParserInitialize);
#endif
}

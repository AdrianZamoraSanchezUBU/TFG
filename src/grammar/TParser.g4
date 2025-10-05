parser grammar TParser;

options {
  tokenVocab = TLexer;
}

/* Main structures */
program: stmt*;

block: LBRACE stmt* RBRACE;

stmt
	: expr
	;

expr
	: expr op=(MUL|DIV) expr
	| expr op=(PLUS|MINUS) expr
  	| operand
  	| LPAREN expr RPAREN
  	;

operand
	: literal
	| IDENTIFIER
	;

literal
	: NUMBER_LITERAL   	
	| FLOAT_LITERAL	
   	;

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
	| expr comparisonOperator expr
  	| operand
  	| LPAREN expr RPAREN
  	;

comparisonOperator
	: EQ 
   	| NE
	| LT 
  	| LE 
   	| GT
   	| GE
   	;

operand
	: literal
	| IDENTIFIER
	;

literal
	: NUMBER_LITERAL   	
	| FLOAT_LITERAL	
   	;

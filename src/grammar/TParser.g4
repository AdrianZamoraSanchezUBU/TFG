parser grammar TParser;

options {
  tokenVocab = TLexer;
}

/* Main structures */
program: stmt*;

block: LBRACE stmt* RBRACE;

stmt
	: expr SEMICOLON
	;

expr
	: expr op=(MUL|DIV) expr		# arithmeticExpr
	| expr op=(PLUS|MINUS) expr		# arithmeticExpr
	| expr comparisonOperator expr	# logicalExpr
  	| operand						# operandExpr
  	| LPAREN expr RPAREN			# parenExpr
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

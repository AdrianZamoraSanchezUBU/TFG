parser grammar TParser;

options {
  tokenVocab = TLexer;
}

/* Main structures */
program: block ;

block: LBRACE stmt* RBRACE ;

stmt: expr SEMICOLON
	| functionDeclaration
	| functionCall SEMICOLON
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
	| STRING_LITERAL
   	;

variableDec : type IDENTIFIER ;

functionDeclaration: type FUNCTION IDENTIFIER LPAREN params? RPAREN block ;

functionCall: IDENTIFIER LPAREN (operand (COMMA operand)*)? RPAREN ;

params: type IDENTIFIER (COMMA type IDENTIFIER)* ;

type
	: TYPE_INT
	| TYPE_FLOAT
	| TYPE_CHAR
	| TYPE_STRING
	| TYPE_BOOLEAN
	;
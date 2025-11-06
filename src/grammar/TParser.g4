parser grammar TParser;

options {
  tokenVocab = TLexer;
}

/* Main structures */
program: programMainBlock ;

programMainBlock: (stmt | return_stmt)* ; 

block: LBRACE (stmt | return_stmt)* RBRACE ;

return_stmt: RETURN expr? SEMICOLON ;

stmt
	: variableDec SEMICOLON
	| variableAssign SEMICOLON
	| functionCall SEMICOLON 
	| functionDeclaration SEMICOLON
	| functionDefinition
	| if
	| loop 
	| expr SEMICOLON
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
	| functionCall
	;

literal
	: NUMBER_LITERAL	
	| FLOAT_LITERAL	
	| STRING_LITERAL
	| boolean_literal
   	;

variableDec: type IDENTIFIER ;

variableAssign 
	: IDENTIFIER ASSIGN_OPERATOR expr
	| variableDec ASSIGN_OPERATOR expr
	;

functionDefinition: type FUNCTION IDENTIFIER LPAREN params? RPAREN block ;

functionDeclaration: type FUNCTION IDENTIFIER LPAREN params? RPAREN  ;

functionCall: IDENTIFIER LPAREN (expr (COMMA expr)*)? RPAREN ;

params: type IDENTIFIER (COMMA type IDENTIFIER)* ;

if: IF LPAREN expr RPAREN block (ELSE else)?;

else
    : if
    | block
    ;

loop
	: WHILE LPAREN expr RPAREN block
	| FOR LPAREN variableAssign SEMICOLON expr SEMICOLON variableAssign RPAREN block
	;

type
	: TYPE_INT
	| TYPE_FLOAT
	| TYPE_CHAR
	| TYPE_STRING
	| TYPE_BOOLEAN
	;

boolean_literal 
				: BOOL_TRUE_LITERAL
				| BOOL_FALSE_LITERAL
				;
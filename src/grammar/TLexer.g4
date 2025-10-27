lexer grammar TLexer;

options {
    caseInsensitive = false;
}

/* KEYWORDs */
FUNCTION : 'function' ;

// Types
TYPE_INT    : 'int' ;
TYPE_FLOAT  : 'float' ;
TYPE_CHAR   : 'char' ;
TYPE_STRING : 'string' ;
TYPE_BOOLEAN : 'bool' ;

/* SYMBOLS */
PLUS  : '+' ;
MINUS : '-' ;
MUL   : '*' ;
DIV   : '/' ;

EQ : '==' ;
NE : '!=' ;
LT : '<' ;
LE : '<=' ;
GT : '>' ;
GE : '>=' ;
ASSIGN_OPERATOR : '=' ;

LPAREN : '(' ;
RPAREN : ')' ;

LBRACE : '{' ;
RBRACE : '}' ;

SEMICOLON : ';' ;
COMMA : ',' ;

/* COMPONENTS */
fragment DIGIT  : [0-9] ;
fragment LETTER : [a-zA-Z] ;

IDENTIFIER     : LETTER+ DIGIT* LETTER* ;
STRING_LITERAL : '"' ~["]* '"';
NUMBER_LITERAL : DIGIT+ ;
FLOAT_LITERAL  : DIGIT+ '.' DIGIT+ ;

BOOL_TRUE_LITERAL   : 'true'  ;
BOOL_FALSE_LITERAL : 'false' ;

/* BLANKS */
COMMNET : '//' ~[\r\n]* -> skip ;
NEWLINE : [\r\n]+ -> skip ;
WS      : [ \t]+ -> skip ;

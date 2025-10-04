lexer grammar TLexer;

options {
    caseInsensitive = false;
}

/* SYMBOLS */
PLUS  : '+' ;
MINUS : '-' ;
MUL   : '*' ;
DIV   : '/' ;

LPAREN : '(' ;
RPAREN : ')' ;

LBRACE : '{' ;
RBRACE : '}' ;

/* COMPONENTS */
fragment DIGIT  : [0-9] ;
fragment LETTER : [a-zA-Z] ;

IDENTIFIER     : LETTER+ DIGIT* LETTER* ;
NUMBER_LITERAL : DIGIT+ ;
FLOAT_LITERAL  : DIGIT+ '.' DIGIT+ ;

/* BLANKS */
COMMNET : '//' ~[\r\n]* -> skip ;
NEWLINE : [\r\n]+ -> skip ;
WS      : [ \t]+ -> skip ;

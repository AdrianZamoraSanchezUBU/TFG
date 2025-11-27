lexer grammar TLexer;

options {
    caseInsensitive = false;
}

/* KEYWORDs */
FUNCTION : 'function' ;
RETURN   : 'return'   ;

IF   : 'if'   ;
ELSE : 'else' ;

WHILE    : 'while'    ;
FOR      : 'for'      ;
CONTINUE : 'continue' ;
BREAK    : 'break'    ;

EVERY : 'every' ;
AT    : 'at'    ;
AFTER : 'after' ;
WHEN  : 'when'  ;
EXIT  : 'exit'  ;
EVENT : 'event' ;

// Types
TYPE_INT     : 'int'    ;
TYPE_FLOAT   : 'float'  ;
TYPE_CHAR    : 'char'   ;
TYPE_STRING  : 'string' ;
TYPE_BOOLEAN : 'bool'   ;
TYPE_VOID    : 'void'   ;
TYPE_PTR     : 'ptr'    ;
TYPE_TIME    : 'time'   ;

BOOL_TRUE_LITERAL  : 'true'  ;
BOOL_FALSE_LITERAL : 'false' ;

TIME_TICK : 'tick' ;
TIME_SEC  : 'sec'  ;
TIME_MIN  : 'min'  ;
TIME_HR   : 'hr'   ;

/* SYMBOLS */
PLUS  : '+' ;
MINUS : '-' ;
MUL   : '*' ;
DIV   : '/' ;

EQ : '==' ;
NE : '!=' ;
LT : '<'  ;
LE : '<=' ;
GT : '>'  ;
GE : '>=' ;
ASSIGN_OPERATOR : '=' ;

LPAREN : '(' ;
RPAREN : ')' ;

LBRACE : '{' ;
RBRACE : '}' ;

SEMICOLON : ';' ;
COMMA     : ',' ;

/* COMPONENTS */
fragment DIGIT  : [0-9] ;
fragment LETTER : [a-zA-Z] ;

IDENTIFIER     : LETTER+ DIGIT* LETTER* ;
STRING_LITERAL : '"' ~["]* '"';
NUMBER_LITERAL : DIGIT+ ;
FLOAT_LITERAL  : DIGIT+ '.' DIGIT+ ;

/* BLANKS */
COMMNET : '//' ~[\r\n]* -> skip ;
NEWLINE : [\r\n]+ -> skip ;
WS      : [ \t]+ -> skip ;

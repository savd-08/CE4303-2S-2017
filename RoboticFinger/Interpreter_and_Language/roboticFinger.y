%{

#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* s);
%}

%union {
	int ival;
	float fval;
}

%token<ival> T_INT
%token<fval> T_FLOAT
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_LEFT T_RIGHT
%token T_NEWLINE T_QUIT
%token T_PUSH T_DRAG T_TOUCH
%token T_POSI T_POSF T_COMMA
%token T_POS T_TIME


%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE

%type<ival> expression

%start program

%%

program: 
	   | program statementList 
;

statementList: T_NEWLINE 
    | statement T_NEWLINE 
    | expression T_NEWLINE { printf("\tResult: %i\n", $1); } 
    | T_QUIT T_NEWLINE { printf("bye!\n"); exit(0); }
;




statement: pushStatement | touchStatement | dragStatement
;

pushStatement: T_PUSH T_POS T_LEFT T_INT  T_COMMA T_INT T_RIGHT T_COMMA  T_TIME T_INT

;

touchStatement: T_TOUCH T_POS T_LEFT T_INT T_COMMA T_INT T_RIGHT ;

dragStatement: T_DRAG T_POSI T_LEFT T_INT T_COMMA T_INT T_RIGHT T_COMMA   
T_POSF T_LEFT T_INT T_COMMA T_INT T_RIGHT

;



expression: T_INT				{ $$ = $1; }
	  | expression T_PLUS expression	{ $$ = $1 + $3; }
	  | expression T_MINUS expression	{ $$ = $1 - $3; }
	  | expression T_MULTIPLY expression	{ $$ = $1 * $3; }
	  | T_LEFT expression T_RIGHT		{ $$ = $2; }
;



%%




void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}

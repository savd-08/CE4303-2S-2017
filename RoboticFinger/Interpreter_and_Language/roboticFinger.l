%option noyywrap

%{
#include <stdio.h>

#define YY_DECL int yylex()

#include "roboticFinger.tab.h"

%}

%%

[ \t]	; // ignore all whitespace
[0-9]+\.[0-9]+ 	{yylval.fval = atof(yytext); return T_FLOAT;}
[0-9]+		{yylval.ival = atoi(yytext); processPosition(yylval.ival);  return T_INT;}
\n		{ sendToDriver(); return T_NEWLINE;}
"+"		{return T_PLUS;}
"-"		{return T_MINUS;}
"*"		{return T_MULTIPLY;}
"/"		{return T_DIVIDE;}
"("		{return T_LEFT;}
")"		{return T_RIGHT;}
"exit"		{return T_QUIT;}
"quit"		{return T_QUIT;}
"push" { setTypeOperation(yytext); return T_PUSH; }
"drag" { setTypeOperation(yytext); return T_DRAG; }
"touch" { setTypeOperation(yytext); return T_TOUCH; }
"posI" { return T_POSI; }
"posF" { return T_POSF; }
"pos" { return T_POS; }
"time" {  return T_TIME; }


"," { return T_COMMA; }









%%

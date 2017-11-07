%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim.tab.h"

// Regex for decimal numbers
%}

decimal	([0-9])+
%%
	//Language keywords & return value, defined in y.tab.h

("MOVE"|"move")			{return INST_MOVE;}
("PIN"|"pin")			{return INST_PIN;}
("PUSH"|"push")			{return INST_PUSH;}
("TOUCH"|"touch")		{return INST_TOUCH;}
("TAM"|"tam")			{return INST_TAM;}


[ \t]					//Ignore tabs and whitespace
[\n]					{return NEWLINE;}		
{decimal}					{yylval.ival = atoi(yytext); return NUM;}
.						{return *yytext;}		

%%

//Called once yylex() reaches the end of the file
int yywrap()
{
	return 1;
}

%{

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "robotic_lib.h"

//Flex/Bison external variables
extern int yylex();
extern int yyparse();
extern FILE *yyin;

FILE *err_log;		//Error log

int line_no = 1;	//Line being analyzed
int t = 0;			// Time to be pushed the current position
int curr_x = 0;		// Current x coordinate
int curr_y = 0;		// Current y coordinate
int x = 0;			// x coordinate of the matrix
int y = 0;			// y coordenate of the matrix
int p = 0;			// Pin to dial

%}

%union {int ival; char *string;};

//Grammar tokens
%start program
%token INST_MOVE
%token INST_PIN
%token INST_PUSH
%token INST_TOUCH
%token INST_TAM
%token NUM
%token NEWLINE

%%

program: 
	instruction newline program			
	| NEWLINE program
	|
	;

newline: 
	NEWLINE					{line_no++;}
	;

instruction:
	instr_move 
	| instr_pin			
	| instr_push						
	| instr_touch
	| instr_tam
	;

instr_touch:
	INST_TOUCH				{touch();}
	;

instr_push:
	INST_PUSH time				{
							if (t > 255)
							{
								yyerror("Push time cannot exceed 255 seconds");							
							}
							else
							{
								push(t);
							}							
						}
	;

instr_move:
	INST_MOVE x_target ',' y_target 		{move(x, y);}
	;

instr_pin:
	INST_PIN pin				{printf("PIN %d\n", p);}
	;

instr_tam:
	INST_TAM x_target ',' y_target 		{touch();move(x,y);}
	;

time:
	NUM					{t = $<ival>1;}
	;

x_target:
	NUM					{
							curr_x = x;
							x = $<ival>1;
						 	if (x > 2)
							{
								yyerror("Invalid X coordinate");
								x = 0;
							}
						}
	;

y_target:
	NUM					{
							curr_y = y;
							y = $<ival>1;
						 	if (y > 3)
							{
								yyerror("Invalid Y coordinate");
								y = 0;
							}
						}
	;

pin:
	NUM					{p = $<ival>1;}
	;
%%

//Reports errors in the error log
void yyerror(const char *s) 
{
	fprintf(err_log, "Error in line %d. Error: %s.\n", line_no, s);
	printf("Error in line %d. Error: %s.\n", line_no, s);
	exit(1);
} 

/* Parses input parameters in order to execute the config file.
	-c is used to set the config file
	-s is used to set the keyboard size (1, 2, or 3 for 1x1cm, 2x2cm and 3x3cm)
	-p is used to set the port
*/
int main(int argc, char **argv) 
{
	//Hardware port
	char *port = NULL;

	//Config file descriptor
	FILE *conf_file;	

	//Config file name
	char *conf_fn = NULL;

	//Keyboard size, default 1x1cm
	int kb_size = 1;

	//Standard getopt() program options parsing
	opterr = 0;
	int c;
	while ((c = getopt (argc, argv, "hc:s:p:")) != -1){
	    switch (c)
		{
		case 'c':
			//Set config file
			conf_fn = optarg;
			break;
		case 's':
			//Set keyboard size
			kb_size = atoi(optarg);
			break;
		case 'p':
			//Set hardware port
			port = optarg;
			break;
		case 'h':
			printf("Usage: %s -c <config_file> -p <hardware_port> [-s <keyboard_size>]\n", argv[0]);
			return;
		case '?':
			//Input error handling
			if (optopt == 'p' )
			  fprintf (stderr, "-%c option requires a valid port.\n", optopt);
			else if (optopt == 's' )
			  fprintf (stderr, "-%c option requires a valid size (1, 2, 3).\n", optopt);
			else if (optopt == 'c' )
			  fprintf (stderr, "-%c option requires a file name.\n", optopt);
			else if (isprint (optopt))
			  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
			  fprintf (stderr,
			           "Unknown option character `\\x%x'.\n",
			           optopt);
			exit(0);
		default:
			abort();
		}
	}

	//Finish if no config file provided
	if (conf_fn == NULL)
	{
		printf("No configuration file provided\n");
		return -1;
	}

	//Finish if fopen error
	conf_file = fopen(conf_fn, "r");
	if (!conf_file)
	{
		printf("Error opening configuration file\n");
		return -1;
	}

	//Finish if fopen errors
	if(!set_port(port)){
		return -1;
	}
	
	//Open the error log
	err_log = fopen("error_log.txt", "w");

	//Set file to be parsed	
	yyin = conf_file;

	//Parse the file
	do 
	{
    	yyparse();
	} 
	while (!feof(yyin));

	//Close open files
	fprintf(err_log, "No errors in file \"%s\"", conf_fn);
	fclose(err_log);
	fclose(conf_file);

	printf("Finished program execution\n");
	return 0;
}

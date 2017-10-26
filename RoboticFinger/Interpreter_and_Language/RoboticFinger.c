

/*
 ============================================================================
 Name        : RoboticFinger.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Main execution File of the Robotic Finger Device
 Reads the values from the configuration file and terminal and makes the use of
 the Device Driver Library to control the Robotic Finger
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Temp File created that is going to be parsed by interpreter
 */

extern FILE *yyin;
/*
 * Path of the config File.
 */
char* CONFIG_FILE ;
/*
 * Maximun Len that a parameter of a config file can read
 */
#define MAXLEN 80

/*
 * Value of the screenSize obtained by the configFile
 */
int sizeScreen;
/*
 * Struct of the values extracted from the configuration file
 */

struct sample_parameters
{
	char item[MAXLEN];
	char flavor[MAXLEN];
	char size[MAXLEN];
}
sample_parameters;

/*
 * Initialization of the Struct
 */

void
init_parameters (struct sample_parameters * parms)
{
	strncpy (parms->item, " ", MAXLEN);
	strncpy (parms->item, " ", MAXLEN);
	strncpy (parms->item, " ", MAXLEN);
}



/*
 * Extract value from the configuration File
 */
char *
trim (char * s)
{
	/* Initialize start, end pointers */
	char *s1 = s, *s2 = &s[strlen (s) - 1];

	/* Trim and delimit right side */
	while ( (isspace (*s2)) && (s2 >= s1) )
		s2--;
	*(s2+1) = '\0';

	while ( (isspace (*s1)) && (s1 < s2) )
		s1++;

	/* Copy finished string */
	strcpy (s, s1);
	return s;
}

/*
 * Main parser function: makes all the logic of the parser
 * and sets the values founded in the sample structure
 *
 */
void
parse_config (struct sample_parameters * parms)
{
	char *s, buff[256];
	FILE *fp = fopen (CONFIG_FILE, "r");

	if (fp == NULL)
	{
		return;
	}

	/* Read next line */
	while ((s = fgets (buff, sizeof buff, fp)) != NULL)
	{
		/* Skip blank lines and comments */
		if (buff[0] == '\n' || buff[0] == '#')
			continue;

		/* Parse name/value pair from line */
		char name[MAXLEN], value[MAXLEN];
		s = strtok (buff, "=");
		if (s==NULL)
			continue;
		else
			strncpy (name, s, MAXLEN);
		s = strtok (NULL, "=");
		if (s==NULL)
			continue;
		else
			strncpy (value, s, MAXLEN);
		printf("Processing \n");
		trim (value);

		/* Copy into correct entry in parameters struct */
		if (strcmp(name, "Size")==0)
			strncpy (parms->item, value, MAXLEN);
		else if (strcmp(name, "Path")==0)
			strncpy (parms->flavor, value, MAXLEN);

		//else
			//printf ("WARNING: %s/%s: Unknown name/value pair!\n",
			//		name, value);
	}

	/* Close file */
	fclose (fp);
}


/*
 * Main function of the program.
 * reads the values from the terminal parameter
 * and the configuration file and sends it to the library
 */
int main(int argc, char *argv[]) {

	int option;
	char* pathConfig;
	while((option = getopt(argc,argv,"p:d:c:q:ftv:"))!=-1)
			{
				switch (option)
				{
					case 'c':
						pathConfig=optarg;

						break;
				}
	}
	//printf("Received Arg %s \n", pathConfig);

	CONFIG_FILE=pathConfig;

	struct sample_parameters parms;

	init_parameters (&parms);
	parse_config (&parms);


	sizeScreen= atoi(parms.item);

	FILE * fp;

	fp = fopen (parms.flavor, "r");
	yyin = fp;


	do {
		yyparse();
	} while(!feof(yyin));

	return 0;
}



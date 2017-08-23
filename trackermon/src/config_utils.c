#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_utils.h"

int load_config_int(char* config_param){
	//Variable declaration
	FILE *fp;
	char grep_cm[64];
	int conf_val;

	//First part of the grep command to extract config params
	strcpy(grep_cm,  "grep ");

	//Append the config param to get
	strcat(grep_cm, config_param);

	//Append the rest of the command
	strcat(grep_cm, " ");
	strcat(grep_cm, CONFIG_FILE);


	//Getting the % from config file
	fp = popen(grep_cm, "r");

	//Exit on file failure
	if (fp == NULL) {
		printf("Could not open config file\n" );
		exit(1);
	}

	//Reading the relevant command output as int
	fscanf(fp, "%*s %d", &conf_val);

	//Close the file
	pclose(fp);

	return conf_val;
}

void load_logfile(char *log_file){
	//Variable declaration
	FILE *fp;
	char grep_cm[64];

	//First part of the grep command to extract config params
	strcpy(grep_cm,  "grep LOGFILE ");

	//Append the rest of the command
	strcat(grep_cm, CONFIG_FILE);

	//Getting the % from config file
	fp = popen(grep_cm, "r");

	//Exit on file failure
	if (fp == NULL) {
		printf("Could not open config file\n" );
		exit(1);
	}

	//Reading the relevant command output as char*
	fscanf(fp, "%*s %s", log_file);

	//Close the file
	pclose(fp);
}

int get_fs_amt(){

	//Variable declaration
	FILE *fp;
	char grep_cm[64];
	int fs_amt;

	//First part of the grep command to extract config params
	strcpy(grep_cm,  "grep -c \"/.* \" ");

	//Append the rest of the command
	strcat(grep_cm, CONFIG_FILE);

	//Getting the % from config file
	fp = popen(grep_cm, "r");

	//Exit on file failure
	if (fp == NULL) {
		printf("Could not open config file\n" );
		exit(1);
	}

	//Reading the relevant command output as char*
	fscanf(fp, "%d", &fs_amt);

	//Close the file
	pclose(fp);

	return fs_amt;
}


int load_fs(int fs_index, char *fs_id){

	//Variable declaration
	FILE *fp;
	char grep_cm[64];
	char buf[32];
	int fs_thresh;

	//itoa(fs_index, buf, 10);
	sprintf(buf,"%d",fs_index);

	//First part of the grep command to extract config params
	strcpy(grep_cm,  "grep \"/.* \" ");

	//Append the rest of the command
	strcat(grep_cm, CONFIG_FILE);
	strcat(grep_cm, " | awk 'NR==");
	strcat(grep_cm, buf);
	strcat(grep_cm, "{print}'");

	//Getting the % from config file
	fp = popen(grep_cm, "r");

	//Exit on file failure
	if (fp == NULL) {
		printf("Could not open config file\n" );
		exit(1);
	}

	//Reading the relevant command output as char*
	fscanf(fp, "%s %d", fs_id, &fs_thresh);

	//Close the file
	pclose(fp);

	return fs_thresh;
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "mon_utils.h"

int mem_usage(){

	//Variable declaration
	FILE *fp;
	int mem_usage; 

	//Getting the memory usage from the free command
	fp = popen("free | grep Mem | awk '{print $3/$2 * 100.0}'", "r");

	//Exit on file failure
	if (fp == NULL) {
		printf("Could not get memory usage\n" );
		exit(1);
	}

	//Reading the command output as a double
	fscanf(fp, "%d", &mem_usage);

	//Close the file
	pclose(fp);

	//Casting to int
	return mem_usage;
}

int fs_usage(char* fs){

	//Variable declaration
	FILE *fp;
	char fs_usage_cm[64];
	int fs_usage;

	//First part of the df command for fs usage
	strcpy(fs_usage_cm,  "df --output=pcent ");

	//Append the fs mount dir
	strcat(fs_usage_cm, fs);

	//Getting the fs usage from the df command
	fp = popen(fs_usage_cm, "r");

	//Exit on file failure
	if (fp == NULL) {
		printf("Could not get filesystem usage\n" );
		exit(1);
	}

	//Reading the relevant command output as int
	fscanf(fp, "%*s %d", &fs_usage);

	//Close the file
	pclose(fp);

	return fs_usage;
}


int cpu_usage(){

	//Variable declaration
    unsigned long  int user_prev, nice_prev, system_prev, st_idle_prev, 
    iowait_prev, irq_prev, sirq_prev, steal_prev;

    unsigned long  int user, nice, system, st_idle, 
    iowait, irq, sirq, steal;

    unsigned long int non_idle_prev, idle_prev, idle, non_idle,
    total_prev, total, delta_total, delta_idle, cpu_usage;

    FILE *fp;

    //Opening /proc/stat as a stream in order to capture CPU time data
    //File descritpion in link
    //https://github.com/Leo-G/DevopsWiki/wiki/How-Linux-CPU-Usage-Time-and-Percentage-is-calculated
    fp = fopen("/proc/stat","r");

    //Exit on file failure
    if (fp == NULL) {
    	printf("Could not get CPU usage\n" );
    	exit(1);
  	}

  	//Reading the relevant variables from /proc/stat
    fscanf(fp,"%*s %lu %lu %lu %lu %lu %lu %lu %lu", 
        &user_prev, &nice_prev, &system_prev, &st_idle_prev, &iowait_prev,
        &irq_prev, &sirq_prev, &steal_prev);

    //Close the file
    fclose(fp);
    
    //Sleep for a second so that we have a time difference
    sleep(1);

    //Opening /proc/stat again to get new CPU time data
    fp = fopen("/proc/stat","r");

    //Exit on file failure
    if (fp == NULL) {
    	printf("Could not get CPU usage\n" );
    	exit(1);
  	}

    //Reading the relevant variables from /proc/stat
    fscanf(fp,"%*s %lu %lu %lu %lu %lu %lu %lu %lu", 
        &user, &nice, &system, &st_idle, &iowait,
        &irq, &sirq, &steal);

    //Close the file
    fclose(fp);

    //Calculating total idle time for both instants
    idle_prev = st_idle_prev + iowait_prev;
    idle = st_idle + iowait;

    //Calculating total busy time for both instants
    non_idle_prev = user_prev + nice_prev + system_prev + irq_prev + sirq_prev + steal_prev;
    non_idle = user + nice + system + irq + sirq + steal;

    //Calculating total time for both instants
    total_prev = non_idle_prev + idle_prev;
    total = non_idle + idle;

    //Calculating idle time difference
    delta_idle = idle - idle_prev;

    //Calculating total time difference
    delta_total = total - total_prev;

    //Calculating CPU usage ((total usage time)/(total time))*100

    cpu_usage = (delta_total - delta_idle)*100;
    cpu_usage /= delta_total; 

    //Casting to int
    return (int)cpu_usage;

}

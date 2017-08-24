#include "mon_utils.h"
#include "config_utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

	//Critial messages processed
	int crit_proc = 0;

	//Loading the log file location
	char log_file[64] = {0};
	load_logfile(log_file);

	//File pointer
	FILE *lf;

	//Getting the amount of filesystems to monitor
	int fs_amt = get_fs_amt();

	//Creating a struct to hold the info for those fs
	struct fs_data fs_list[fs_amt];

	//Load info from the config file for each fs
	for(int i = 0; i < fs_amt; i++){
		fs_list[i].thresh = load_fs(i+1, fs_list[i].id);
	}

	//Loading CPU threshold from the config file
	int cpu_thresh = load_config_int("CPUthreshold");

	//Loading CPU threshold from the config file
	int mem_thresh = load_config_int("Memthreshold");

	while(1){
		//Get CPU and memory usage
		int cpu_level = cpu_usage();
		int mem_level = mem_usage();

		//Alert if CPU usage is equal or greater than its threshold
		if(cpu_level >= cpu_thresh){
			
			//printf("[CRITICAL] – CPU usage is currently %d which is over %d.\n", cpu_level, cpu_thresh);

			//Log the event
			lf = fopen(log_file, "a+");
			fprintf(lf, "[CRITICAL] – CPU Usage is currently %d which is over %d\n", cpu_level, cpu_thresh);
			fclose(lf);
		}

		//Alert if memory usage is equal or greater than its threshold
		if(mem_level >= mem_thresh){
			
			//printf("[CRITICAL] – Memory usage is currently %d which is over %d.\n", mem_level, mem_thresh);

			//Log the event
			lf = fopen(log_file, "a+");
			fprintf(lf, "[CRITICAL] – Memory usage is currently %d which is over %d.\n", mem_level, mem_thresh);
			fclose(lf);
		}

		//Measuring usage level for each fs listed in the config file
		for(int i = 0; i < fs_amt; i++){
			//Get fs usage
			int fs_level = fs_usage(fs_list[i].id);

			//Alert if fs usage is equal or greater than its threshold
			if(fs_level >= fs_list[i].thresh){
				
				//printf("[CRITICAL] – File system %s usage is currently %d which is over %d.\n", fs_list[i].id, fs_level, fs_list[i].thresh);

				//Log the event
				lf = fopen(log_file, "a+");
				fprintf(lf, "[CRITICAL] – File system %s usage is currently %d which is over %d.\n", fs_list[i].id, fs_level, fs_list[i].thresh);
				fclose(lf);
			}
		}

		//Get the current amount of critical messages
		int crit_msg = crit_msg_amt();

		//Log new messages only
		for(crit_proc; crit_proc < crit_msg; crit_proc++){
			//Message buffer
			char msg[MSG_MAX_LEN] = {0};

			//Get a message by number
			sys_msg(msg, crit_proc + 1);

			//printf("System critical error has been detected: %s", msg);

			//Log the message
			lf = fopen(log_file, "a+");
			fprintf(lf, "System critical error has been detected: %s", msg);
			fclose(lf);
		}

		//Pause execution for 2 seconds
		sleep(2);
	}

	//------------------------------TESTS---------------------------------

	/*for(int i = 0; i < fs_amt; i++){
		printf("%d\n",fs_usage(fs_list[i].id));
	}

	/*printf("%d\n",cpu_usage());
	printf("%d\n",mem_usage());
	printf("%d\n",fs_usage("/"));
	printf("%d\n",load_config_int("Memthreshold"));
	printf("%s\n", log_file);*/
	
	return(0);
}

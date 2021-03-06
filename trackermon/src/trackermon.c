#include "mon_utils.h"
#include "config_utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[]){
	//Process ID and Session ID
    pid_t process_id, sid;
        
    process_id = fork();

    //Testing fork
    if (process_id < 0) 
	{
            //Return exit failure
            exit(1);
    }

    if (process_id > 0) 
	{
            //Return exit success to kill parent process
            exit(0);
    }

    //Setting umask permission to 0
    umask(0);
            
    //Create a new sesion to give independence to the daemon
    sid = setsid();
    if (sid < 0) 
	{
            //Exit on failure
            exit(1);
    }
    
    // Change the current working directory to root
    chdir("/");
    
    //Close stdin, stdout and stderr for security purposes 
    int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
    int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
    int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2

    //Loading the log file location
	char log_file[64] = {0};
	load_logfile(log_file);

	//Log file opened here    
    FILE *lf = fopen(log_file, "w");   
    
	// Since we're not writing to the file yet we keep it closed
	fclose(lf);

	//Critial messages processed
	int crit_proc = 0;


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

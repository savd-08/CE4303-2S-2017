#include "mon_utils.h"
#include "config_utils.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
	//Loading the log file location
	char log_file[64] = "";
	load_logfile(log_file);

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

	//------------------------------TESTS---------------------------------

	for(int i = 0; i < fs_amt; i++){
		printf("%d\n",fs_usage(fs_list[i].id));
	}

	/*printf("%d\n",cpu_usage());
	printf("%d\n",mem_usage());
	printf("%d\n",fs_usage("/"));
	printf("%d\n",load_config_int("Memthreshold"));
	printf("%s\n", log_file);*/
	
	return(0);
}
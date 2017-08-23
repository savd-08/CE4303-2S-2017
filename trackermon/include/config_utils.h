#ifndef _CONFIG_UTILS_H
#define _CONFIG_UTILS_H

#define CONFIG_FILE "config.conf"

struct fs_data {
	char id[32];
	int thresh;
};

int load_config_int(char* config_param);

void load_logfile(char *log_file);

int get_fs_amt();

int load_fs(int fs_index, char *fs_id);

#endif
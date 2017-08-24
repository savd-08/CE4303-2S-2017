#ifndef _MON_UTILS_H
#define _MON_UTILS_H

#define MSG_MAX_LEN 256

int mem_usage();

void sys_msg(char *msg, int msg_index);

int crit_msg_amt();

int fs_usage(char* fs);

int cpu_usage();

#endif
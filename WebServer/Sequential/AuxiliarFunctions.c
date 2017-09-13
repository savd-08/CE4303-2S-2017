#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"log.h"
#include <semaphore.h>
#include <pthread.h>
pthread_mutex_t lock_requests_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_size_mtx     = PTHREAD_MUTEX_INITIALIZER;

static int total_requests;
static int total_size;

/*
 * Boolean function that verifies if the file exists
 */


void increment_request()
{
//	pthread_mutex_lock(&lock_requests_mtx);
	total_requests++;
//	pthread_mutex_unlock(&lock_requests_mtx);
}

int show_total_requests()
{
	return total_requests;
}

int check_file_exists(const char * path)
{
	FILE * fp;
	if(fp=fopen(path,"r"))
	{
		fclose(fp);
		return 1;
	}
	return 0;
}

/*
 * Obtains the size of the file
 */
int file_size(const char *path)
{
	if(!check_file_exists(path)) exit(0);

	struct stat st;
	stat(path,&st);
	return st.st_size;
}
/*
 * Checks if the folder is a proper file system
 */

int check_folder_exists(const char *path)
{
	struct stat st;
	if(lstat(path,&st)<0)
	{	print_log(ERROR,"ROOT DIRECTORY DOES NOT EXISTS");
		return 0;
	}
	if(S_ISDIR(st.st_mode))
		return 1;
}

/*
 *
 * Set the index of the server as the file index.html
 */

set_index(char *path)
{
	struct stat st;
	if(lstat(path,&st)<0)
	{	perror("");
		return -1;
	}
	if(S_ISDIR(st.st_mode))  strcat(path,"/index.html");
	return 1;
}

/*
 * Extracts the specified file of the url made by the client
 */
void trim_resource(char * resource_location)
{
	if(strstr(resource_location,"#")) strcpy(strpbrk(resource_location,"#"),"");
	if(strstr(resource_location,"?")) strcpy(strpbrk(resource_location,"?"),"");
}


void add_to_total_size(int new_size)
{
	total_size += new_size;
}

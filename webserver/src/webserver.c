#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include "HttpStructure.h"


static  char  	      path_root[PATH_MAX];
static  int   	      port_number         = DEFAULT_PORT_NUMBER; //8011
static	int 		  pre_amt = 10;
typedef void 	      (*strategy_t)(int);
static  sig_atomic_t  status_on           = True;
static  char  	      strategy_name[STRATEGY_MAX];
timer_t total_uptime,requests_time;
log_t current_log_no = DEBUG;

pthread_mutex_t pt_lock;

static int total_requests;
static int total_size;

// Gets the log id

int get_log_id()
{
	return current_log_no;
}

// Prints the log in terminal

int print_log(log_t log_level,char * fmt, ...)
{
	if(log_level > get_log_id())
		return 0;
	int print_count;
	va_list myargs;
	va_start(myargs,fmt);
	print_count = vfprintf(stderr,fmt,myargs);
	va_end(myargs);
	return print_count;
}

// Boolean function that verifies if the file exists

void increment_request()
{
	total_requests++;
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

// Obtains the size of the file

int file_size(const char *path)
{
	if(!check_file_exists(path)) exit(0);

	struct stat st;
	stat(path,&st);
	return st.st_size;
}

// Checks if the folder is a proper file system

int check_folder_exists(const char *path)
{
	struct stat st;
	if(lstat(path,&st)<0)
	{	print_log(ERROR,"ROOT DIRECTORY DOES NOT EXIST");
		return 0;
	}
	if(S_ISDIR(st.st_mode))
		return 1;
}

// Set the index of the server as the file index.html

int set_index(char *path)
{
	struct stat st;
	if(lstat(path,&st)<0)
	{	perror("");
		return -1;
	}
	if(S_ISDIR(st.st_mode))  strcat(path,"/index.html");
	return 1;
}

// Extracts the specified file of the url made by the client

void trim_resource(char * resource_location)
{
	if(strstr(resource_location,"#")) strcpy(strpbrk(resource_location,"#"),"");
	if(strstr(resource_location,"?")) strcpy(strpbrk(resource_location,"?"),"");
}

void add_to_total_size(int new_size)
{
	total_size += new_size;
}

//  Verifies the request sent by a client, like headers(data requested)

static int verify_request(int fd,http_request_t *request)
{
	char   command_line  [MAX_HEADER_LINE_LENGTH];
	char   method        [MAX_METHODS];
	char   payload_source[PATH_MAX];
	int    minor_version;
	int    major_version;
	char   head_name     [MAX_HEADER_LINE_LENGTH];
	char   head_value    [MAX_HEADER_VALUE_LENGTH];
	int    head_count = 0;
	FILE * fr;
	fr = fdopen(dup(fd),"r");

	if(fr)
	{
		fgets(command_line,MAX_HEADER_LINE_LENGTH,fr);
		sscanf(command_line, "%s %s HTTP/%d.%d%*s",method,payload_source,&major_version,&minor_version);

		if(strcmp(method,"GET")==0)
		{
			request->method        = HTTP_METHOD_GET;
			trim_resource(payload_source);
			strcpy(request->uri,payload_source);
			request->major_version = major_version;
			request->minor_version = minor_version;
		}
		else
		{
			request->method        = HTTP_STATUS_NOT_IMPLEMENTED;
		}

	}
	while(head_count < MAX_HEADERS)
	{
		fgets(command_line,MAX_HEADER_LINE_LENGTH,fr);

		if(strstr(command_line,":"))
		{
			sscanf(command_line,"%s: %s",head_name,head_value);

			strcpy(request->headers[head_count].field_name,head_name);
			strcpy(request->headers[head_count++].field_value,head_value);
		}
		else
			break;
	}

	request->header_count=head_count;
	fclose(fr);
	return 1;
}

/*
 * Sets the values of the header of the request
 *  that is going to be sent back to the client
 */

static int set_headerValues(http_response_t *response,const char *name,const char *value)
{
	strcpy(response->headers[response->header_count].field_name,name);
	strcpy(response->headers[response->header_count++].field_value,value);
	return 1;
}

//  Function that handles the error of the request.

static int handle_error(http_status_t status,char * error_resource)
{
	if(status.code == HTTP_STATUS_LOOKUP[HTTP_STATUS_OK].code)	return 1;
	if(status.code == HTTP_STATUS_LOOKUP[HTTP_STATUS_NOT_FOUND].code)
	{
		strcpy(error_resource,path_root);
		strcat(error_resource,ERROR_NOT_FOUND_404);

		if(!check_file_exists(error_resource))
		{
			strcpy(error_resource,path_root);
			strcat(error_resource,ERROR_BAD_REQUEST_400);
		}
		if(!check_file_exists(error_resource))
		{
			strcpy(error_resource,ERROR_NOT_FOUND_404);
		}
	}
	return 0;
}

//  Verifies if the requested file exists, and builds the response


static http_status_t check_response_status(const int status,const char * path)
{
	if(status == HTTP_STATUS_NOT_IMPLEMENTED) return HTTP_STATUS_LOOKUP[status];
	if(!check_file_exists(path))              return HTTP_STATUS_LOOKUP[HTTP_STATUS_NOT_FOUND];
	return HTTP_STATUS_LOOKUP[HTTP_STATUS_OK];
}

/*
 * Method that returns the type of header of the html reponse depending
 * of the requested file
 */

const char* getFileExtension(const char *path)
{
	/* html */
	if(strstr(path,".HTML") || strstr(path,".html")) return "text/html";
	if(strstr(path,".HTM" ) || strstr(path,".htm" )) return "text/html";
	if(strstr(path,".XML" ) || strstr(path,".xml" )) return "application/xml";
	if(strstr(path,".CSS" ) || strstr(path,".css" )) return "text/css";
	if(strstr(path,".JS"  ) || strstr(path,".js"  )) return "application/javascript";
	/* images */
	if(strstr(path,".GIF" ) || strstr(path,".gif" )) return "image/gif";
	if(strstr(path,".JPEG") || strstr(path,".jpeg")) return "image/jpeg";
	if(strstr(path,".PNG" ) || strstr(path,".png" )) return "image/png";
	if(strstr(path,".JPG" ) || strstr(path,".jpg" )) return "image/jpg";
	if(strstr(path,".ICO" ) || strstr(path,".ico" )) return "image/ico";
	/* video */
	if(strstr(path,".MPEG") || strstr(path,".mpeg")) return "video/mpeg";
	if(strstr(path,".MPG" ) || strstr(path,".mpg" )) return "video/mpeg";
	if(strstr(path,".MP4" ) || strstr(path,".mp4" )) return "video/mp4";
	if(strstr(path,".MOV" ) || strstr(path,".mov" )) return "video/quicktime";
	if(strstr(path,".MKV" ) || strstr(path,".mkv" )) return "video/mkv";
	if(strstr(path,".OGV" ) || strstr(path,".ogv" )) return "video/ogg";
	/* files */
	if(strstr(path,".TXT" ) || strstr(path,".txt" )) return "text";
	if(strstr(path,".PDF" ) || strstr(path,".pdf" )) return "application/x-pdf";
	if(strstr(path,".ZIP" ) || strstr(path,".zip" )) return "image/zip";
	if(strstr(path,".GZ"  ) || strstr(path,".gz"  )) return "image/gz";
	if(strstr(path,".TAR" ) || strstr(path,".tar" )) return "image/tar";
	/* music */
	if(strstr(path,".MP3" ) || strstr(path,".mp3" )) return "audio/mpeg";
	if(strstr(path,".OGG" ) || strstr(path,".ogg" )) return "audio/ogg";
	if(strstr(path,".WAV" ) || strstr(path,".wav" )) return "audio/mp3";
	if(strstr(path,".OPUS") || strstr(path,".opus")) return "audio/oggg";

	return "text/html";
}

/*
 * Basically builds the header of the response that is going
 * to be sent to the client
 */

static int build_response(const http_request_t *request,http_response_t *response)
{
	char buffer[MAX_HEADER_VALUE_LENGTH];
	int head_count=0;
	time_t now=0;
	struct tm *t;
	strcat(response->resource_path,request->uri);
	set_index(response->resource_path);

	response->status        = check_response_status(request->method,response->resource_path);
	handle_error(response->status, response->resource_path);

	response->major_version = request->major_version;
	response->minor_version = request->minor_version;

	now = time(NULL);
	t = gmtime(&now);
	strftime(buffer,30,"%a, %d %b %Y %H:%M:%S %Z",t);

	set_headerValues(response,"Date",buffer);
	set_headerValues(response,"Server","CSUC HTTP");
	set_headerValues(response,"Content-Type",getFileExtension(response->resource_path));
	sprintf(buffer,"%d",file_size(response->resource_path));
	add_to_total_size(atoi(buffer));
	set_headerValues(response,"Content-Length",buffer);

	return 1;
}

// Prepares the file that is going to be sent

static int send_response(int fd,const http_response_t *response)
{
	time_t now;
	struct tm t;
	FILE   *fr;
	char   buf[MAX_HEADER_VALUE_LENGTH];
	FILE   *fp = fdopen(dup(fd),"w");
	size_t size;
	int    head_no;
	int    ch;
	fprintf(fp,"HTTP/%d.%d %d %s\r\n",response->major_version,response->minor_version,response->status.code,response->status.reason);
	for(head_no = 0; head_no<response->header_count; head_no++)
	{
		fprintf(fp,"%s: %s\r\n",response->headers[head_no].field_name,response->headers[head_no].field_value);
	}
	fprintf(fp,"\n");
	fr=fopen(response->resource_path,"r");  //print payload
	if(fr)
	{
		while((ch=getc(fr))!=EOF)  fprintf(fp,"%c",ch);
		fclose(fr);
	}
	fclose(fp);
	return 1;
}

// Clears the header for the next response

static int clear_responses(http_response_t *response)
{
	int head_no;
	for(head_no =0; head_no<response->header_count; head_no++)
	{
		strcpy(response->headers[head_no].field_name,"");
		strcpy(response->headers[head_no].field_value,"");
	}
	response->header_count = 0;
	return 1;
}

/*
 * Request administrator function
 * Calls all the functions required to make the response to the client
 */

 static int next_request(int fd,http_request_t *request)
{
	char   command_line  [MAX_HEADER_LINE_LENGTH];
	char   method        [MAX_METHODS];
	char   payload_source[PATH_MAX];
	int    minor_version;
	int    major_version;
	char   head_name     [MAX_HEADER_LINE_LENGTH];
	char   head_value    [MAX_HEADER_VALUE_LENGTH];
	int    head_count = 0;
	FILE * fr;
	fr = fdopen(dup(fd),"r");

	if(fr)
	{
		fgets(command_line,MAX_HEADER_LINE_LENGTH,fr);
		sscanf(command_line, "%s %s HTTP/%d.%d%*s",method,payload_source,&major_version,&minor_version);

		if(strcmp(method,"GET")==0)
		{
			request->method        = HTTP_METHOD_GET;
			trim_resource(payload_source);
			strcpy(request->uri,payload_source);
			request->major_version = major_version;
			request->minor_version = minor_version;
		}
		else
		{
			request->method        = HTTP_STATUS_NOT_IMPLEMENTED;
		}
	}
	while(head_count < MAX_HEADERS)
	{
		fgets(command_line,MAX_HEADER_LINE_LENGTH,fr);

		if(strstr(command_line,":"))
		{
			sscanf(command_line,"%s: %s",head_name,head_value);

			strcpy(request->headers[head_count].field_name,head_name);
			strcpy(request->headers[head_count++].field_value,head_value);
		}
		else
			break;
	}
	request->header_count=head_count;
	fclose(fr);
	increment_request();
	//print_log(DEBUG,"\n Request number :%d",show_total_requests());
	return 1;
}


static void manage_single_request(int peer_sfd)
{
	http_request_t  *request  = (http_request_t*)malloc(sizeof(http_request_t));
	http_response_t *response = (http_response_t*)malloc(sizeof(http_response_t));
	strcpy(response->resource_path,path_root);

	next_request(peer_sfd, request);
	build_response(request, response);
	send_response(peer_sfd, response);

	clear_responses(response);
	free(request);
	free(response);
}

// Create the process for each request

static void perform_serially(int sfd)
{
	//print_log(DEBUG,"Sequential Version");
	while(status_on)
	{
		int peer_sfd = accept(sfd,NULL,NULL);

		if(peer_sfd == -1)
		{
			print_log(WARNING,"\nAccept failed");
			continue;
		}
		manage_single_request(peer_sfd);

		close(peer_sfd);
	}
}

//manage the request in a child process
static void forked_request(int peer_sfd) {
	manage_single_request(peer_sfd);

	close(peer_sfd);
}

static void listen_forked(int sfd) {
	//avoid the creations of zombies
	struct sigaction sigchld_action = {
	  .sa_handler = SIG_DFL,
	  .sa_flags = SA_NOCLDWAIT
	};
	sigaction(SIGCHLD, &sigchld_action, NULL);
	//process id for the fork
	pid_t pid;

	while (status_on) {
		//obtains the sockect conection
		int peer_sfd = accept(sfd,NULL,NULL);
		//if the conections fails
		if(peer_sfd == -1){
			continue;
		}
		//if the conections works it makes a fork
		pid = fork();

		if(pid < 0){
			perror("the fork failed");
			exit(1);
		}
		//for the fork process
		else if(pid == 0){
			close(sfd);
			forked_request(peer_sfd);
			exit(0);
		}
		//for the parent process
		else{
			close(peer_sfd);
		}
	}
}

static void listen_preforked(int sfd){
	//forks amount
	int forks_amt = pre_amt;
	//actual child
	int act_child = 0;
	//process id
	pid_t pid;

	//initializes the forks
	for(int i =0; i < forks_amt; i++){
		//creates a child
		pid = fork();
		if(pid < 0){
			perror("forks fails");
			exit(1);
		}else if(pid == 0){
			pid = i;
			break;
		}
	}

	if(pid > forks_amt){
		while(status_on){
			//obtains the sockect conection
			int peer_sfd = accept(sfd,NULL,NULL);
			//if the conections fails
			if(peer_sfd == -1){
				continue;
			}
			pid = fork();
			//if fork fails
			if(pid < 0){
        perror("Error on fork");
        exit(1);
			}
			else if(pid == 0){
				close(sfd);
				forked_request(peer_sfd);
			}else{
				close(peer_sfd);
			}
		}
		close(sfd);
	}

	while(status_on){
		//obtains the sockect conection
		int peer_sfd = accept(sfd,NULL,NULL);
		//if the conections fails
		if(peer_sfd == -1){
			continue;
		}
		if(pid == act_child){
			forked_request(peer_sfd);
		}
		close(peer_sfd);
		act_child++;
		if(act_child >= forks_amt){
			act_child = 0;
		}
	}

	close(sfd);

	exit(0);
}

//Function to be called by a thread when executed
void *manage_request_thread(void *v_ptr){
	//Cast the void pointer to int
	int peer_sfd = *((int *) v_ptr);

	//printf("Thread %lu joined \n", pthread_self());

	//Free the memory being used by v_ptr
	free(v_ptr);

	//Free resources once the thread finishes
	pthread_detach(pthread_self());

	//Call the request attention function
	manage_single_request(peer_sfd);

	//Close the socket
	close(peer_sfd);

	//Finish thread execution
	pthread_exit(NULL);
}

static void listen_threaded(int sfd)
{
	int peer_sfd;
	//Infinite loop waiting for requests
	while(status_on)
	{
		//Wait for a request on socket sfd (accept is a blocking function)
		peer_sfd = accept(sfd, NULL, NULL);

		//If sfd < 0, accept function has thrown an error
		if(peer_sfd < 0)
		{
			print_log(ERROR,"Couldn't accept request\n");
			continue;
		}

      	//Pointer to an sfd so that we have a separate sfd for our thread
      	int *other_sfd = malloc(sizeof *other_sfd);

      	//Don't do anything if pointer creation fails
		if(other_sfd == NULL){
			  print_log(ERROR,"Error creating pointer of socket\n");
			  continue;
		}

		//Assign the sfd value to the pointer
		*other_sfd = peer_sfd;

		//Pointer to a thread
      	pthread_t request_thread;

		//Create the pthread using the manage_request_thread function and other_sfd as a parameter
      	if(pthread_create(&request_thread, NULL, manage_request_thread, other_sfd) < 0) {
			//Do nothing if failure
			printf("Error creating thread\n");
			//Free the memory being used by other_sfd
			free(other_sfd);
			//Close the socket
			close(peer_sfd);
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}

//Function to be called by a thread when executed
void *manage_request_prethread(void *v_ptr){
	//Cast the void pointer to int
	int sfd = *((int *) v_ptr);

	//printf("Thread %lu \n", pthread_self());

	while(status_on)
	{
		//Lock mutex
		pthread_mutex_lock(&pt_lock);

		//Wait for a request in the sfd socket
		int peer_sfd = accept(sfd, NULL, NULL);

		//Unlock mutex
		pthread_mutex_unlock(&pt_lock);

		//Continue loop if accept failed
		if(peer_sfd == -1)
		{
			print_log(WARNING,"\nAccept failed");
			continue;
		}

		//Manage a single request
		manage_single_request(peer_sfd);

		close(peer_sfd);
	}

}

static void listen_prethreaded(int sfd)
{
	//Pointer to a thread
    pthread_t request_thread[pre_amt];

    //Initialize mutex
    if (pthread_mutex_init(&pt_lock, NULL) != 0)
    {
        print_log(ERROR,"\n mutex init failed\n");
        exit(1);
    }

    //Create all pre_amt threads
    for (int i = 0; i < pre_amt; ++i)
    {
    	//Create the pthread using the manage_request_prethread function and other_sfd as a parameter
	    if(pthread_create(&request_thread[i], NULL, manage_request_prethread, &sfd) < 0) {
			//Do nothing if failure
			printf("Error creating thread\n");
			//Close the socket
			close(sfd);
			pthread_exit(NULL);
		}
	 }

	pthread_exit(NULL);
}


//  Initialize the sockets of the server

int initialize_server()
{
	struct sockaddr_in myaddr;
	int    sfd;
	int    optval = 1;

	sfd = socket(AF_INET,SOCK_STREAM,0);    //creating socket

	if(sfd == -1)
	{
		print_log(ERROR,"socket");
		exit(0);
	}

	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		print_log(WARNING,"\nsetsockopt");

	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family      = AF_INET;
	myaddr.sin_port        = htons(port_number);
	myaddr.sin_addr.s_addr = INADDR_ANY;

	sigset_t new;
    sigemptyset (&new);
    sigaddset(&new, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &new, NULL) != 0)
    {
        print_log(ERROR,"Unable to mask SIGPIPE");
        exit(-1);
	}

	if(bind(sfd, (struct sockaddr*) &myaddr, sizeof(myaddr)) == -1)
	{
		print_log(ERROR,"Port not found:");
		exit(0);
	}
	if(listen(sfd,BACKLOG)==-1)
		print_log(WARNING,"\nFailed listening to socket.");

	return sfd;
}

// Configure the parameters of the server like path of the files, etc

strategy_t configure_server(int argc,char *argv[], int *pre_amt)
{
	//Define a strategy_t variable (function pointer)
	strategy_t operation = &perform_serially;

	//Option count
	int opt_count = 0;

	int c;

	//Set opterr to zero, as required by getopt
	opterr = 0;

	//Capture all program options
	while ((c = getopt (argc, argv, "fstr:d:")) != -1 && opt_count == 0){
	    switch (c)
		{
		case 'f':
			//Set function pointer to forked version
			operation = &listen_forked;
			port_number = FORKED_PORT_NUMBER; //port
			strcpy(strategy_name,"Forked");
			break;
		case 's':
			//Set function pointer to sequential version
			operation = &perform_serially;
			port_number = DEFAULT_PORT_NUMBER; //port
			strcpy(strategy_name,"Sequential(FIFO)");
			break;
		case 't':
			//Set function pointer to threaded version
			operation = &listen_threaded;
			port_number = THREADED_PORT_NUMBER; //port
			strcpy(strategy_name,"Threaded");
			break;
		case 'r':
			//Set function pointer to pre-threaded version
			operation = &listen_prethreaded;
			//Set thread amount
			*pre_amt = atoi(optarg);
			strcpy(strategy_name,"Prethreaded");
			port_number = PRETHREADED_PORT_NUMBER; //port
			break;
		case 'd':
			//Set function pointer to pre-forked version
			operation = &listen_preforked;
			//Set fork amount
			*pre_amt = atoi(optarg);
			strcpy(strategy_name,"Preforked");
			port_number = PREFORKED_PORT_NUMBER; //port
			break;
		case '?':
			if (optopt == 'r' || optopt == 'd' )
			  fprintf (stderr, "Option -%c requires an integer argument.\n", optopt);
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
		//Take first option into account only
		opt_count++;
	}

	//Check if the root folder exists
	strcpy(path_root,"./files");
	if(!check_folder_exists(path_root))
		exit(0);

	return operation;
}

//  Main function that calls all the functions required to create server

int main(int argc,char *argv[])
{
	strategy_t server_operation;
	int sfd;
	server_operation = (strategy_t)configure_server(argc, argv, &pre_amt);
	sfd              = initialize_server();
	server_operation(sfd);  				//start server
	if(close(sfd)==-1)					//close server
		fprintf(stderr,"Error while closing.\n");
	return 0;
}

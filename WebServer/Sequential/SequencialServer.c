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
#include <signal.h>
#include "HttpStructure.h"


static  char  	      path_root[PATH_MAX];
static  int   	      port_number         = DEFAULT_PORT_NUMBER; //8011
typedef void 	      (*strategy_t)(int);
static  sig_atomic_t  status_on           = True;
static  char  	      strategy_name[STRATEGY_MAX];
timer_t total_uptime,requests_time;
log_t current_log_no = DEBUG;

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
	{	print_log(ERROR,"ROOT DIRECTORY DOES NOT EXISTS");
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
	print_log(DEBUG,"\n Request number :%d",show_total_requests());
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
	print_log(DEBUG,"Sequencial Version");
	while(status_on)
	{
		int peer_sfd = accept(sfd,NULL,NULL);                           

		if(peer_sfd == -1)
		{	
			print_log(WARNING,"\nACCEPT FAILED MAY BE BECAUSE OF INTERRUPT");
			continue;
		}
		manage_single_request(peer_sfd);
		
		close(peer_sfd);
	}
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

	if(bind(sfd, (struct sockaddr*) &myaddr, sizeof(myaddr)) == -1)
	{
		print_log(ERROR,"Puerto no encontrado:");
		exit(0);
	}
	if(listen(sfd,BACKLOG)==-1)
		print_log(WARNING,"\n Failed listen socket");

	return sfd;
}

// Configure the parameters of the server like path of the files, etc

strategy_t configure_server(int argc,char *argv[])
{
	int option,option_count=0;
	strategy_t operation;

	strcpy(path_root,"./files");

	strcpy(strategy_name,"Sequencial(FIFO)");
	operation = &perform_serially;
	option_count++;

	if(option_count > 1)
	{
		print_log(ERROR,"\nDon't pass arguments to use more than one strategy.\n");
		exit(0);
	}

	if(!check_folder_exists(path_root)) exit(0);


	return operation;
}

//  Main function that calls all the functions required to create server

int main(int argc,char *argv[])
{
	strategy_t server_operation;
	int sfd;
	server_operation = (strategy_t)configure_server(argc,argv);
	sfd              = initialize_server();
	server_operation(sfd);  				//start server
	if(close(sfd)==-1)					//close server
		print_log(WARNING,"\nError while closing");

	return 0;
}

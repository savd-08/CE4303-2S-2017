/* Client for WebServer C*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int n_cycles, port;
char *host, *file;

#define BUFSIZE 8196999

void *send_request(void *thread_id)
{
	long id;
	id = (long) thread_id;
	int socket_fd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFSIZE];
	int i;
	// for by <N-cycles>
	for (i = 0; i < n_cycles; i++)
	{		
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0) 
		{
			perror("Error opening socket");
      			exit(1);
   		}		
		server = gethostbyname(host);
		
		if (server == NULL) 
		
		{
      			printf("Error: No such host\n");
      			exit(0);
   		}
   		
		bzero((char *) &serv_addr, sizeof(serv_addr));
   		serv_addr.sin_family = AF_INET;
   		bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
   		serv_addr.sin_port = htons(port);
   		
		if (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		{
      			perror("Error connecting");
      			exit(1);
   		}
   		
   		// Obtain the object through GET
		strcpy(buffer, "GET /");
		strcat(buffer, file);		
		strcat(buffer, " HTTP/1.1");
		
		n = write(socket_fd, buffer, BUFSIZE);
		
		if (n < 0) 
		{
      			perror("Error writing to socket");
      			exit(1);
   		}
		
		bzero(buffer,BUFSIZE);
		
   		n = read(socket_fd, buffer, BUFSIZE);
		   
   		if (n < 0) 
		{
			perror("Error reading from socket");
      			exit(1);
   		}
   		//printf("%s", i);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	                           // Arguments the client need to run
	int n_threads;
	if (argc != 6)             // If the total arguments are not provided it exits with an error
	{
		printf("Usage: %s <machine> <port> <file> <N-threads> <N-cycles>\n", argv[0]);
		return -1;
	}
	                           // Set the values to the corresponding arguments
	host = argv[1];            // Name or IP address where the server is running
	port = atoi(argv[2]);      // Port number of the corresponding server
	file = argv[3];            // Path to the file that is going to be transferred
	n_threads = atoi(argv[4]); // That are going to send requests to the corresponding server
	n_cycles = atoi(argv[5]);  // Each thread is going to repeat this request <N-cycles> times

	pthread_t *threads = malloc(sizeof(pthread_t) * n_threads);
	int rc;
	long t;
	for (t = 0; t < n_threads; t++)
	{
		rc = pthread_create(&threads[t], NULL, send_request, (void *) t);
		if (rc) 
		{
			printf("Error creating thread\n");
			exit(-1);
		}
	}

	for (t = 0; t < n_threads; t++)
	{
		pthread_detach(threads[t]);		
	}
	pthread_exit(NULL);
	return 0;
}

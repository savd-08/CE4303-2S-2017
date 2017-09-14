
#ifndef HTTPSTRUCTURES_H_
#define HTTPSTRUCTURES_H_


#include <limits.h>
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
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdarg.h>

/*
 * Constants of http responses
 */
#define URI_MAX                     PATH_MAX
#define MAX_HEADER_NAME_LENGTH      256
#define MAX_HEADER_VALUE_LENGTH     768
#define MAX_HEADER_LINE_LENGTH      1024
#define MAX_HEADERS                 64
#define MAX_METHODS                 10
#define BACKLOG                     50
#define ERROR_BAD_REQUEST_400       "./files/404.html"
#define ERROR_NOT_FOUND_404         "./files/404.html"
#define True                        1
#define False                       0
#define STRATEGY_MAX                20
#define CURRENT_DIRECTORY           "."
#define DEFAULT_WORKER_SIZE         16
#define DEFAULT_BUFFER_SIZE         16
#define DEFAULT_PORT_NUMBER         8011


typedef enum  {
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_GET,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_CONNECT
} http_method_t;



enum http_status {
    HTTP_STATUS_CONTINUE,
    HTTP_STATUS_SWITCHING_PROTOCOLS,

    HTTP_STATUS_OK,
    HTTP_STATUS_CREATED,
    HTTP_STATUS_ACCEPTED,
    HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION,
    HTTP_STATUS_NO_CONTENT,
    HTTP_STATUS_RESET_CONTENT,
    HTTP_STATUS_PARTIAL_CONTENT,

    HTTP_STATUS_MULTIPLE_CHOICES,
    HTTP_STATUS_MOVED_PERMANENTLY,
    HTTP_STATUS_FOUND,
    HTTP_STATUS_SEE_OTHER,
    HTTP_STATUS_NOT_MODIFIED,
    HTTP_STATUS_USE_PROXY,
    HTTP_STATUS_TEMPORARY_REDIRECT,

    HTTP_STATUS_BAD_REQUEST,
    HTTP_STATUS_UNAUTHORIZED,
    HTTP_STATUS_PAYMENT_REQUIRED,
    HTTP_STATUS_FORBIDDEN,
    HTTP_STATUS_NOT_FOUND,
    HTTP_STATUS_METHOD_NOT_ALLOWED,
    HTTP_STATUS_NOT_ACCEPTABLE,
    HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED,
    HTTP_STATUS_REQUEST_TIME_OUT,
    HTTP_STATUS_CONFLICT,
    HTTP_STATUS_GONE,
    HTTP_STATUS_LENGTH_REQUIRED,
    HTTP_STATUS_PRECONDITION_FAILED,
    HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE,
    HTTP_STATUS_REQUEST_URI_TOO_LARGE,
    HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
    HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE,
    HTTP_STATUS_EXPECTATION_FAILED,

    HTTP_STATUS_INTERNAL_SERVER_ERROR,
    HTTP_STATUS_NOT_IMPLEMENTED,
    HTTP_STATUS_BAD_GATEWAY,
    HTTP_STATUS_SERVICE_UNAVAILABLE,
    HTTP_STATUS_GATEWAY_TIME_OUT,
    HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED
};

typedef struct {
    int   code;
    char *reason;
} http_status_t;

const http_status_t HTTP_STATUS_LOOKUP[] = {

    {100, "Continue"},
    {101, "Switching Protocols"},


    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},


    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {307, "Temporary Redirect"},


    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Time-out"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Request Entity Too Large"},
    {414, "Request-URI Too Large"},
    {415, "Unsupported Media Type"},
    {416, "Requested range not satisfiable"},
    {417, "Expectation Failed"},

    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Time-out"},
    {505, "HTTP Version not supported"}
};

typedef struct {
    char field_name[MAX_HEADER_NAME_LENGTH];
    char field_value[MAX_HEADER_VALUE_LENGTH];
} http_header_t;

typedef struct {
    int            method;
    char           uri[URI_MAX];
    int            major_version;
    int            minor_version;
    http_header_t  headers[MAX_HEADERS];
    int            header_count;
} http_request_t;

typedef struct {
    http_status_t  status;
    int            major_version;
    int            minor_version;
    http_header_t  headers[MAX_HEADERS];
    int            header_count;
    char           resource_path[PATH_MAX];
} http_response_t;


/*
 * Structures of the server_log like id and name
 */
typedef enum
{
	ERROR = 0,
	WARNING,
	INFO,
	DEBUG
}server_log_id_t;

typedef int log_t;

typedef struct
{
	int id;
	char *name;
}server_log_t;


#endif /* HTTPSTRUCTURES_H_ */
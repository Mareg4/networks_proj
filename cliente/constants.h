#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h> 
#include <netdb.h>

#define TRUE 1
#define FALSE 0

#define MAX_RESPONSE 4
#define SV_LOG_MAX_SIZE 7
#define BUFFER_SIZE 128
#define MAX_FILENAME 25
#define MAX_TRYS 3
#define MAX_TIME 600
#define NULL_PLID -1
#define MAX_6CHAR_NUMBER 999999
#define TCP_HEADER_SIZE_SB 32
#define TCP_HEADER_SIZE_ST 28
#define LOCALHOST "127.0.0.1"
#define DEFAULT_PORT "58026"
#define CMD_SIZE 16
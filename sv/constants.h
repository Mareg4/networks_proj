#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h> 



#define GN 26
#define DEFAULT_PORT (58000 + GN)
#define TRUE 1
#define FALSE 0
#define MYPORT "58026"

#define BUFFER_SIZE 128
#define MAX_SV_LOG 64
#define MAX_TRYS 3
#define MAX_TIME 600
#define NULL_PLID -1
#define MAX_6CHAR_NUMBER 999999
#define NUM_CORES 6
#define MAX_TCP_CONNECTIONS 10
#define SELECT_TIMEOUT_SEC 10
#define MAX_SV_STD_RESPONSE 16
#define GAME_NOT_STARTED -1 
#define MODEDEBUG 'D'
#define MODEPLAY 'P'
#define SB_SIZE 10
#define TIME_OVER 0
#define TRY_SEQ_SIZE 4
#define CMD_SIZE 3

#define TCP_HEADER_SIZE_SB 56
#define TCP_HEADER_SIZE_ST 51
#define MAX_SCORE 100
#define TIME_SCORE_WEIGHT (0.5 * MAX_SCORE)
#define COLORS_SCORE_WEIGHT (0.5 * MAX_SCORE)
#define HALF_RIGHT_COLORS_SCORE_WEIGHT (0.5 * COLORS_SCORE_WEIGHT)
#define MAX_F_SIZE 1024

#define WIN 'W'
#define LOSS 'L'

#define CORES "RBYPOG"

#ifndef MAIN_H
#define MAIN_H

#include <constants.h>
#include <player_def.h>
#include <aux.h>
#include <protocols.h>


extern Player_list* head;
// --- UDP Connection constants -------
int fd;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;

// -----


/**
 * divide os args reecebidos do terminal
 */
void parse_args(int argc, char *argv[]);

#endif
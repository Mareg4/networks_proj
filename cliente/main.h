#ifndef MAIN_H
#define MAIN_H

#include <constants.h>
#include <player_def.h>
#include <signal.h>

// --- Connection constants -------
int fd; // UDP
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;

// -----

#endif

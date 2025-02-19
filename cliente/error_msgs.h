#ifndef MAIN_H
#define MAIN_H

#include <constants.h>
#include <player_def.h>
#include <aux.h>

extern Player player;

void err_try_cmd(const char sv_protocol[BUFFER_SIZE]);

#endif
#ifndef PROTOCOLS_RESPONSES_H
#define PROTOCOLS_RESPONSES_H

#include <constants.h>
#include <player_def.h>
#include "aux.h"

/**
 * Manda o RSG OK
 */
void done_SNG(struct sockaddr_in Addr, int fd);

/**
 * Erro de SNG
 */
void err_SNG(char* err_msg, struct sockaddr_in Addr, int fd);

/**
 * Quando o modo de debug funciona
 */
void done_DBG(struct sockaddr_in Addr, int fd);

/**
 * Quando o modo de debug tem um erro
 */
void err_DBG(char* err_msg, struct sockaddr_in Addr, int fd);

/**
 * Manda RQT OK
 */
void done_QUT(char* solution, struct sockaddr_in Addr, int fd);

/**
 * Quando o quit da erro
 */
void err_QUT(char* err_msg, struct sockaddr_in Addr, int fd);

/**
 * Manda RTR OK
 */
void done_TRY(struct sockaddr_in Addr, int fd);

/**
 * Quando existe um erro a fazer Try
 */
void err_TRY(char* err_msg, struct sockaddr_in Addr, int fd);

/**
 * Manda STR OK
 */
void done_STR(int fd);

/**
 * Quando existe um erro a fazer Show Trials
 */
void err_STR(char* err_msg, int fd);

/**
 * Manda SSB OK
 */
void done_SSB(int fd);

/**
 * Quando existe um erro a fazer Show Scoreboard
 */
void err_SSB(char* err_msg, int fd);

#endif
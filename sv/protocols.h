#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include "constants.h"
#include "player_def.h"
#include "gameEngine.h"
#include "protocols_responses.h"
#include "aux.h"

extern Player_list* head;
extern int v_mode;

/**
 * Verificar se o tempo está válido (se ainda esta a decorrer), se o tempo acabar faz end_game e retorna TIME_OVER
 */
int updt_time(Player* p);

/**
 * Verifica se a cor recebida é valida/existe
 */
int valid_color(char cor);

/**
 * recebe o protocolo SNG e inicializa o jogo caso nao haja jogo ativo associado ao player
 */
int SNG_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd);

/**
 * recebe o protocolo DBG e inicializa o jogo em modo debug caso nao haja jogo ativo associado ao player
 */
int DBG_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd);

/**
 * recebe o protocolo TRY e faz a tentativa correspondente 
 */
int TRY_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd);

/**
 * Recebe o protocolo QUT e elimina o jogador
 */
int QUT_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd);

/**
 * Recebe o protocolo SSB e envia scoreboard
 */
int SSB_protocol(int fd);

/**
 * Recebe o protocolo STR e envia os trials
 */
int STR_protocol(char client_protocol[BUFFER_SIZE], int fd);

#endif 

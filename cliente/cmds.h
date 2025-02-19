#ifndef CMDS_H
#define CMDS_H

#include <constants.h>

/**
 * @brief funcao que lida com o start
 */
int cmd_start(char input[BUFFER_SIZE], char sv_protocol[BUFFER_SIZE]);

/**
 * @brief funcao que lida com o  try
 */
int cmd_try(char input[BUFFER_SIZE], char sv_protocol[BUFFER_SIZE]);

/**
 * @brief funcao que lida com o debug
 */
int cmd_debug(char input[BUFFER_SIZE], char sv_protocol[BUFFER_SIZE]);

/**
 * @brief funcao que lida com o showtrials
 */
int cmd_st(char sv_protocol[BUFFER_SIZE]);

/**
 * @brief funcao que lida com o scoreboard
 */
int cmd_sb(char sv_protocol[BUFFER_SIZE]);

/**
 * @brief funcao que lida com o quit
 */
int cmd_quit(char sv_protocol[BUFFER_SIZE]);

/**
 * @brief funcao que lida com o exit
 */
void cmd_exit(char sv_protocol[BUFFER_SIZE]);

/**
 * @brief pergunta ao utilizadoe se quer mesmo sair
 */
int aux_confirmacao_saida();

/**
 * cria um ficheiro e escreve a string content
 */
void create_file(const char *file_name, const char *content);


#endif
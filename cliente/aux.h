#ifndef AUX_H
#define AUX_H

#include <constants.h>
#include <player_def.h>


// --- UDP Connection constants -------
extern int fd;
extern socklen_t addrlen;
extern struct addrinfo hints, *res;
extern struct sockaddr_in addr;

// -----

extern Player player;

extern char* IP;
extern char* PORT;

// -----


typedef struct {
    int     inited;
    size_t  file_size;
    char    file_name[MAX_FILENAME];
    int     empty;
    char*   file_content;
} responseTCP;


// -----

/**
 * inicializa o jogador (nao ter PLID e 1 gameTry)
 */
void init_player(Player* p);

/**
 * ler input duh
 */
void read_input(char input[BUFFER_SIZE]);

/**
 * @brief o comando incial ("start", "try", etc...)
 */
void get_cmd(char input[BUFFER_SIZE], char cmd[CMD_SIZE]);

/**
 * @brief que verifica o maximo de tentativas de conexao
 */
void aux_connect(char* IP, char* PORT, char buffer[BUFFER_SIZE]);

/**
 * @brief funcao que conecta com o servidor
 */
int try_connect(char* IP, char* PORT, char buffer[BUFFER_SIZE]);

/**
 * @brief funcao que manda a mensagem/protocolo especifico ao servidor
 */
int send_udp_msg(char sv_protocol[BUFFER_SIZE]);

/**
    @brief Receber do servidor e caso nao de tem 3 tentativas
 */
int receive_udp_msg(char sv_protocol[BUFFER_SIZE]);

/**
    @brief Receber do servidor uma mensagem tcp
 */
responseTCP receive_tcp_msg(char sv_protocol[BUFFER_SIZE], unsigned int header_size, int fd_tcp);

/**
    @brief Enviar para o servidor uma mensagem/protocolo tcp
 */
int send_tcp_msg(char sv_protocol[BUFFER_SIZE], int fd_tcp);

/**
    @brief Receber e enviar de/para servidor mensagem tcp
 */
responseTCP two_way_tcp(char sv_protocol[BUFFER_SIZE], int header_size);

/**
 * Lê n_bytes no read e retorna o que leu
 */
ssize_t simple_read_tcp(char* sv_protocol, size_t n_bytes, int fd_tcp);

/**
 * conectar TCP sockets e tal
 */
int connect_tcp();

#endif
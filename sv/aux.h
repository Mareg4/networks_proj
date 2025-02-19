#ifndef AUX_H
#define AUX_H

#include "constants.h"
#include "player_def.h"
#include "gameEngine.h"
#include "tentativa.h"
extern Player_list* head;
extern int PORT;

/**
 * Descobrir o ip do servidor
 */
void get_svIP();

/**
 * Inicializar sockets de tcp e fazer bind
 */
void init_tcp(int* tcp_fd);

/**
 * Inicializar spckets de udp e fazer bind
 */
void init_udp(int* udp_fd);

/**
 * Recebe a resposta do cliente (3 way handshake)
 */
void init_udp_talk(int fd, char client_protocol[BUFFER_SIZE], struct sockaddr_in addr);

/**
 * retorna apenas o protocolo inicial
 */
void get_cmd(char input[BUFFER_SIZE], char cmd[CMD_SIZE+1]);

/**
 * imprimir uma tentativa especifica (debug)
 */
void imprimir_tentativa(const Tentativa* tentativa);

/**
 * Procura o ultimo jogo (mais recente) de um jogador que esta num ficheiro
 */
int FindLastGame(char *plid, char *fname);

/**
 * Escreve o cabeçalho de um arquivo de jogo
 */
int writeHeader(Player *player, char mode);

/**
 * Regista uma try no arquivo de jogos
 */
int writeTry(Tentativa* t, Player* p);

/**
 * Escrever rodape
 */
struct tm *writeFooter_and_saveFile(Player *p, char win_or_loss);

/**
 * Dar renameea um ficheiro e polo no sitio especifico plyer
 */
void rename_and_move_file(char *file_path, char *new_name, char *plid);

/**
 * Cria um diretório para o player
 */
void createDir(int path);

/**
 * Verifica se uma tentiva já foi feita por um player especifico
 */
int tentativaJaExiste(char* sequencia, Player* player_atual);

/**
 * Finaliza o jogo, salva a pontuação e dá reset ao jogador
 */
void end_game(Player *p, char win_or_loss);

/**
 * Faz print dos jogadores todos e das respetivas tentativas
 */
void print_player_list(Player_list* head);

/**
 * Obtém as 10 melhores classificações duma lista de players
 */
int FindTopScores(SCORELIST *list);

/**
 * Retorna o numero de ficheiros scoreboard
 */
int sb_file_count();

/**
 * Cria a string a ser enviada com a ScoreBoard
 */
int create_ST_str(Player *p, char filename[128], char* buffer);

#endif // AUX_H

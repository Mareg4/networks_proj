#ifndef PLAYER_DEF_H
#define PLAYER_DEF_H

#include <gameEngine.h>
#include <constants.h>
#include <tentativa.h>


typedef struct {
    struct sockaddr_in addr;    // Endereço IP e porta do jogador
    int PLID;                   // Player ID
    int gameStarted;            // bool 0 - Nao começou 1 - Começou
    time_t timeOfStart;         // tempo em q o jogo comecou
    time_t timeOfEnd;           // tempo em q o jogo terminará por falta de tempo
    char solution[5];           // solucao do jogo
    Tentativa tentativas[8];    // Lista de até 8 tentativas (max)
    int numTentativas;          // idx da lista de tentativas
    int score;                  // score de 0-100
    char mode;                  // Modo de debug ou normal
} Player;


typedef struct Player_list Player_list;

struct Player_list { // lista ligada de players
    Player player;           
    Player_list* next; 
}; 

typedef struct {
    int  score[SB_SIZE];                // Armazena as pontuações dos jogadores
    char PLID[SB_SIZE][7];              // Identificadores dos jogadores
    char col_code[SB_SIZE][5];          // Cor ou similar
    int  notries[SB_SIZE];              // Número de tentativas para cada jogador
    char  mode[SB_SIZE];                 // Modo do jogo (e.g., MODEPLAY, MODEDEBUG)
    int  nscores;                       // Número total de pontuações armazenadas
} SCORELIST;



/**
 * adiciona um player a lista de players ativos, se o jogador ainda estiver com o jogo a decorrer retorna FALSE
 */
int add_player(int PLID, int timeLeft, struct sockaddr_in addr);

/**
 *  Da reset aos valores da struct player
 */
void reset_player(int PLID);

/**
 * encontrar jogador atraves do PLID
 */
Player* find_player_by_PLID(int PLID);

/**
 * encontrar um player atraves do seu IP
 */
Player* find_player_by_IP(struct sockaddr_in addr);

/**
 * remover jogador atraves do PLID
 */
int remove_player(int PLID);

/**
 * adiciona uma tentativa ao player especifico, se exceder as 8 tentativas retorna FALSE
 */
int add_tentativa(Tentativa tentativa, Player* player_atual);

/**
 * Elimina todos os players da lista de players
 */
void eliminate_all_players();

#endif // PLAYER_DEF_H

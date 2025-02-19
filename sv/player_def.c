#include <player_def.h>

pthread_mutex_t player_list_mutex;
Player_list* head = NULL;


int add_player(int PLID, int timeLeft, struct sockaddr_in addr) {

    Player_list* current = head;
    while (current != NULL) {
        if ((current->player.PLID == PLID) && (current->player.gameStarted == TRUE)) {       // se ja tem jogo a decorrer
            return FALSE;
        }
        else if ((current->player.PLID == PLID) && (current->player.gameStarted == FALSE)) { // se ja existe player mas o jogo nao esta a decorrer
            
            char solucao[5];              
            gerar_solucao(solucao);

            current->player.PLID            = PLID;
            current->player.numTentativas   = 0;
            current->player.gameStarted     = TRUE;
            current->player.timeOfStart     = time(NULL);
            current->player.timeOfEnd       = current->player.timeOfStart + timeLeft;
            current->player.score           = 0;
            current->player.mode            = '\0'; 

            

            strncpy(current->player.solution, solucao, 5);
           
            //printf("%s\n", current->player.solution);
            current->player.addr           = addr;
            
            return TRUE;
        }  
        current = current->next;
    }
    
    // novo player
    Player_list* new_node = (Player_list*)malloc(sizeof(Player_list));
    if (new_node == NULL) {
        printf("Erro a mallocar espaco para player List\n");
        return FALSE;
    }

    char solucao[4];
    gerar_solucao(solucao);
    
    new_node->player.PLID           = PLID;
    new_node->player.gameStarted    = TRUE;
    new_node->player.timeOfStart    = time(NULL);
    new_node->player.timeOfEnd      = time(NULL) + ((time_t)timeLeft);
    new_node->player.score          = 0;
    new_node->player.mode           = MODEPLAY;
    

    
    
    strncpy(new_node->player.solution, solucao, 5);
    //printf("%s\n", new_node->player.solution);
    new_node->player.addr           = addr;
    
    new_node->player.numTentativas  = 0;
    new_node->next                  = head;
    
    head = new_node; // inserir no início da lista
    

    return TRUE;
}


Player* find_player_by_PLID(int PLID) {

    Player_list* current = head;
    while (current != NULL) {
        if (current->player.PLID == PLID) {
            
    
            return &current->player; // Retorna o ponteiro para o jogador encontrado
        }
        current = current->next;
    }


    return NULL;
}

Player* find_player_by_IP(struct sockaddr_in addr) {

    Player_list* current = head; 
    while (current != NULL) {
        
        if (current->player.addr.sin_addr.s_addr == addr.sin_addr.s_addr &&
            current->player.addr.sin_port == addr.sin_port) {

    
            return &current->player; // Retorna o ponteiro para o jogador encontrado
        }
        current = current->next;
    }
    return NULL; // Retorna NULL se nenhum jogador for encontrado
}


int remove_player(int PLID) {
    
    Player_list* current = head;
    Player_list* previous = NULL;

    while (current != NULL) {
        if (current->player.PLID == PLID) {

            
            if (previous == NULL) {
                // Se for o primeiro nó da lista
                head = current->next;
            } else {
                // Remover nó intermediário ou final
                previous->next = current->next;
            }
            
            free(current);
            return TRUE;
        }
        previous = current;
        current = current->next;
    }
    
    return FALSE;
}


void reset_player(int PLID) {
    Player* p = find_player_by_PLID(PLID);

    p->gameStarted      = FALSE;
    p->numTentativas    = 0;
    p->timeOfStart      = GAME_NOT_STARTED;
    p->timeOfEnd        = GAME_NOT_STARTED;
    p->score            = 0;
    p->mode             = '\0';
    memset(p->solution, '\0', sizeof(p->solution)); 

    
    // limpar as tentativas
    for (int i = 0; i < 8; i++) {
        memset(&p->tentativas[i], 0, sizeof(Tentativa));
    }
    
}

int add_tentativa(Tentativa tentativa, Player* player_atual) {
    
    if (player_atual == NULL) {
        perror("Nao existe esse player");
        return FALSE;
    }

    if (player_atual->numTentativas > 7) return FALSE;

    player_atual->tentativas[player_atual->numTentativas] = tentativa;
    
    player_atual->numTentativas++;
    return TRUE;
}


void eliminate_all_players() {
   
    Player_list* current = head;
    Player_list* temp;

    while (current != NULL) {
        temp = current->next; 
        free(current);        // Liberta o nó atual
        current = temp;       // Avança para o próximo nó
    }

    head = NULL;
}

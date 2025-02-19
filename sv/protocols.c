#include <protocols.h>


int updt_time(Player* p) {
    
    if(p == NULL || !p->gameStarted) return -1; // O player não existe OU o jogo já acabou

    if((p->gameStarted) && (time(NULL) > p->timeOfEnd)) { // tempo expirou

        end_game(p, LOSS);
        return TIME_OVER;
    }
    return TRUE; // dentro do tempo
}

int valid_color(char cor) {
    for(u_int8_t i = 0; i < strlen(CORES); i++) {
        if(CORES[i] == cor) return TRUE; 
    }
    return FALSE;
}


int SNG_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd) {
    int PLID, gameTime;
    char extra = '\0';

    if((sscanf(client_protocol, "SNG %d %d\n%c", &PLID, &gameTime, &extra) != 2) || (extra != '\0')) { 
        printf("[LOG] <SNG>: Args invalidos...\n");
        err_SNG("ERR", addr, fd);
        return FALSE;
    }

    if (gameTime > MAX_TIME) {
        printf("[LOG] <%d>: O tempo fornecido (%ds) não pode exceder 600s.\n", PLID, gameTime); 
        err_SNG("ERR", addr, fd);
        return FALSE;
    }

    if (gameTime < 0) {
        printf("[LOG] <%d>: Tempo não pode ser negativo...\n", PLID);
        err_SNG("ERR", addr, fd);
        return FALSE;
    }

    if (PLID > MAX_6CHAR_NUMBER) {
        printf("[LOG] <%d>: O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", PLID, PLID); 
        err_SNG("ERR", addr, fd);
        return FALSE;
    }

    if (PLID < 0) {
        printf("[LOG] <%d>: O PlayerID fornecido não pode ser negativo.\n", PLID);
        err_SNG("ERR", addr, fd);
        return FALSE;
    }

    if (!add_player(PLID, gameTime, addr)) {
        printf("[LOG] <%d>: Erro SNG. Player tem jogo a decorrer.\n", PLID);
        err_SNG("NOK", addr, fd);
        return FALSE;
    }

    Player *p = find_player_by_PLID(PLID);
    if (p == NULL) {
        perror("Player nao existe");
        return FALSE;
    }

    p->mode = MODEPLAY;
    
    createDir(PLID);
    writeHeader(p, MODEPLAY);

    if(v_mode) printf("[V] <%d>: Started game!\n", PLID);
    done_SNG(addr, fd);
    
    return TRUE;
}

int DBG_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd) {
    int plid, gameTime;
    char cor1, cor2, cor3, cor4, extra = '\0';

    if((sscanf(client_protocol, "DBG %d %d %c %c %c %c\n%c", &plid, &gameTime, &cor1, &cor2, &cor3, &cor4, &extra) != 6) || (extra != '\0')) { 
        printf("[LOG] <DBG>: Args invalidos...\n");
        err_DBG("ERR", addr, fd);
        return FALSE;
    }

    if (gameTime > MAX_TIME) {
        printf("[LOG] <%d>: O tempo fornecido (%ds) não pode exceder 600s.\n", plid, gameTime); 
        err_DBG("ERR", addr, fd);
        return FALSE;
    }

    if (gameTime < 0) {
        printf("[LOG] <%d>: Tempo não pode ser negativo...\n", plid);
        err_DBG("ERR", addr, fd);
        return FALSE;
    }

    if (plid > MAX_6CHAR_NUMBER) {
        printf("[LOG] <%d>: O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", plid, plid); 
        err_DBG("ERR", addr, fd);
        return FALSE;
    }

    if (plid < 0) {
        printf("[LOG] <%d>: O PlayerID fornecido não pode ser negativo.\n", plid);
        err_DBG("ERR", addr, fd);
        return FALSE;
    }

    if(!valid_color(cor1) || !valid_color(cor2) || !valid_color(cor3) || !valid_color(cor4)) {
        printf("[LOG] <%d>: As cores fornecidas não são válidas.\n", plid);
        err_DBG("ERR", addr, fd);
        return FALSE;
    }

    if (!add_player(plid, gameTime, addr)) {
        printf("[LOG] <%d>: Erro SNG. Player tem jogo a decorrer.\n", plid);
        err_DBG("NOK", addr, fd);
        return FALSE;
    }

    Player* p = find_player_by_PLID(plid);
    if (p == NULL) {
        perror("Player nao existe");
        return FALSE;
    }
    
    p->solution[0] = cor1;
    p->solution[1] = cor2;
    p->solution[2] = cor3;
    p->solution[3] = cor4;
    p->mode        = MODEDEBUG;

    createDir(plid);
    writeHeader(find_player_by_PLID(plid), MODEDEBUG);


    if(v_mode) printf("[V] <%d>: Started DEBUG game with solution '%c %c %c %c'!\n", plid, cor1, cor2, cor3, cor4);
    done_DBG(addr, fd);

    return TRUE;
}


int TRY_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd) {
    
    Tentativa* tentativa = (Tentativa*) malloc(sizeof(Tentativa));
    
    char cor1, cor2, cor3, cor4, extra = '\0';
    int n_try, plid;

    if ((sscanf(client_protocol, "TRY %d %c %c %c %c %d/n%c",\
    &plid, &cor1, &cor2, &cor3, &cor4, &n_try, &extra) != 6) || (extra != '\0')) {
        printf("[LOG] <TRY> Args invalidos...\n");
        err_TRY("ERR", addr, fd);
        return FALSE;
    }

    if (plid > MAX_6CHAR_NUMBER) {
        printf("[LOG] <%d>: O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", plid, plid); 
        err_TRY("ERR", addr, fd);
        return FALSE;
    }

    if (plid < 0) {
        printf("[LOG] <%d>: O PlayerID fornecido não pode ser negativo.\n", plid);
        err_TRY("ERR", addr, fd);
        return FALSE;
    }

    if(!valid_color(cor1) || !valid_color(cor2) || !valid_color(cor3) || !valid_color(cor4)) {
        printf("[LOG] <%d>: As cores fornecidas não são válidas.\n", plid);
        err_TRY("ERR", addr, fd);
        return FALSE;
    }
    
    Player* p = find_player_by_PLID(plid);
    if (p == NULL) {
        perror("Player nao existe");
        return FALSE;
    }
     
    if (updt_time(p) == TIME_OVER) {
        char output[16];
        printf("[LOG] <%d>: O tempo terminou.\n", plid);

        snprintf(output, sizeof(output)-1, "ETM %c %c %c %c",\
        p->solution[0], p->solution[1], p->solution[2], p->solution[3]);
        
        err_TRY(output, addr, fd);

        if(p) reset_player(p->PLID);  // reset ou remove
        return FALSE;
    }
    
    if((p == NULL) || (p->gameStarted == FALSE)) {
        printf("[LOG] <TRY>: O Player não tem nenhum jogo a decorrer.\n");
        err_TRY("NOK", addr, fd);
        return FALSE;
    }

    tentativa->sequencia[0] = cor1;
    tentativa->sequencia[1] = cor2;
    tentativa->sequencia[2] = cor3;
    tentativa->sequencia[3] = cor4;
    tentativa->sequencia[4] = '\0'; 
    
    tentativa->nB = 0;
    tentativa->nW = 0;
    tentativa->num_tentativa = n_try;        

    // o cliente mandou mandou uma repeticao da guess anterior e nao atualiza
    if((n_try == tentativa->num_tentativa - 1) &&\
    (strncmp(tentativa->sequencia, p->tentativas[p->numTentativas - 1].sequencia, sizeof(tentativa->sequencia)))) {
        
        tentativa->num_tentativa--;
        
        printf("[LOG] <TRY>: O número de tentativas não foi incrementado.\n");
        done_TRY(addr, fd);
    }
    
    if(tentativaJaExiste(tentativa->sequencia, p)) {

        printf("[LOG] <%d>: O Player repetiu uma sequência.🍑\n", plid);
        err_TRY("DUP", addr, fd);
        return FALSE;
    }

    if(((n_try == tentativa->num_tentativa - 1) &&\
    (!strncmp(tentativa->sequencia, p->tentativas[p->numTentativas - 1].sequencia, sizeof(tentativa->sequencia)))) ||\
    (n_try != tentativa->num_tentativa)) {
        
        printf("[LOG] <%d>: Meu deus porque?💀💀💀\n", plid);
        err_TRY("INV", addr, fd);
        return FALSE;
    }

    try_function(tentativa, p->solution);       // vai realizar a tentativa e verificar as posicoes certas e tal

    if(!add_tentativa(*tentativa, p)) {
        char output[13];

        printf("[LOG] <%d>: O Player excedeu as 8 tentativas.\n", plid);
        
        snprintf(output, sizeof(output)-1, "ENT %c %c %c %c", p->solution[0], p->solution[1], p->solution[2], p->solution[3]);
        
        err_TRY(output, addr, fd);
        end_game(p, LOSS);
        return FALSE;
    }

    if(v_mode) printf("[V] <%d>: Player fez try.\n", p->PLID);
    writeTry(tentativa, p);

    if(tentativa->nB == 4) {
        printf("[LOG] <%d>: O Player acertou a combinação!\n", plid);
        done_TRY(addr, fd);
        
        end_game(p, WIN);
        return TRUE;
    }
    done_TRY(addr, fd);     // futuramente pode se remover
  
    return TRUE;
}





int QUT_protocol(char client_protocol[BUFFER_SIZE], struct sockaddr_in addr, int fd) {
    int PLID;
    char extra = '\0';

    if((sscanf(client_protocol, "QUT %d\n%c", &PLID, &extra) != 1) || (extra != '\0')) {
        printf("[LOG] <QUT> Args invalidos...\n");
        err_QUT("ERR", addr, fd);
        return FALSE;
    }
    if (PLID > MAX_6CHAR_NUMBER) {
        printf("[LOG] <%d>: O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", PLID, PLID); 
        err_QUT("ERR", addr, fd);
        return FALSE;
    }

    if (PLID < 0) {
        printf("[LOG] <%d>: O PlayerID fornecido não pode ser negativo.\n", PLID);
        err_QUT("ERR", addr, fd);
        return FALSE;
    }


    Player *p = find_player_by_PLID(PLID);
    updt_time(p);
    if((p == NULL) || (p->gameStarted == FALSE)) {
        err_QUT("NOK", addr, fd);
        printf("[LOG] <%d>: O Player não tem nenhum jogo a decorrer.\n", PLID);
        return FALSE;
    }

    if(v_mode) printf("[V] <%d>: Quited game!\n", PLID);
    
    done_QUT(p->solution, addr, fd);
    end_game(p, LOSS);
    return TRUE;
}


// ------------------------------------------ TCP ------------------------------------------------------------

int SSB_protocol(int fd) {

    SCORELIST scoreList;

    FindTopScores(&scoreList);

    char header[TCP_HEADER_SIZE_SB], mode[6];
    char str_msg[MAX_F_SIZE] = "";

    if(v_mode) printf("[V] Um player pediu ScoreBoard.\n");

    if (scoreList.nscores == 0) {
        strcpy(header, "RSS EMPTY\n");
        
        printf("[LOG] <SB>: ScoreBoard vazia.\n");
        write(fd, header, strlen(header));
        return FALSE;
    }

    snprintf(str_msg, sizeof(str_msg), "\n--------------------------------🎄 TOP 10 SCORES 🎁--------------------------------\n\n");
    strncat(str_msg, "                 SCORE PLAYER   CODE   NO TRIALS   MODE\n\n", sizeof(str_msg) - strlen(str_msg) - 1);

    for (int i = 0; i < scoreList.nscores && i < SB_SIZE; i++) {
        char line[128];

        if (scoreList.mode[i] == MODEPLAY) strcpy(mode, "PLAY");
        else strcpy(mode, "DEBUG");

        snprintf(line, sizeof(line), "%13d - %5d  %-7s  %-4s       %d       %s\n",
                 i + 1, 
                 scoreList.score[i], 
                 scoreList.PLID[i], 
                 scoreList.col_code[i],
                 scoreList.notries[i], 
                 mode); 

        strncat(str_msg, line, sizeof(str_msg) - strlen(str_msg) - 1);
    }

    snprintf(header, TCP_HEADER_SIZE_SB, "RSS OK TOPSCORES_%07d.txt %03zu\n", sb_file_count(), strlen(str_msg));

    printf("%s%s", header, str_msg);

    write(fd, header, strlen(header));
    write(fd, str_msg, strlen(str_msg));

    return TRUE;
}

int STR_protocol(char client_protocol[BUFFER_SIZE], int fd) {
    int plid;
    char extra = '\0';

    if((sscanf(client_protocol, "STR %d\n%c", &plid, &extra) != 1) || (extra != '\0')) {
        printf("[LOG] <STR> Args invalidos...\n");
        err_STR("ERR", fd);
        return FALSE;
    }
    if (plid > MAX_6CHAR_NUMBER) {
        printf("[LOG] <%d>: O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", plid, plid); 
        err_STR("ERR", fd);
        return FALSE;
    }
    if (plid < 0) {
        printf("[LOG] <%d>: O PlayerID fornecido não pode ser negativo.\n", plid);
        err_STR("ERR", fd);
        return FALSE;
    }

    if(v_mode) printf("[V] <%d>: Player pediu para mostrar os trials.\n", plid);


    char header[TCP_HEADER_SIZE_ST];
    char str_msg[MAX_F_SIZE - TCP_HEADER_SIZE_ST] = "";
    char game_file_name[128] = "";
    
    Player *p = find_player_by_PLID(plid);
    updt_time(p);
    if((p == NULL) || (p->gameStarted == FALSE)) {
        char str_plid[7];
        
        printf("[LOG] <%d>: O Player não tem nenhum jogo a decorrer.\n", plid);

        snprintf(str_plid, sizeof(str_plid), "%d", plid); // toString

        int exists_last_game = FindLastGame(str_plid, game_file_name);

        if(!exists_last_game) {   // nao ha historico, nem jogo a decorrer rn
            snprintf(header, TCP_HEADER_SIZE_ST, "RSS NOK\n");
            write(fd, header, strlen(header));
            return FALSE;
        }
        else {                      // ha historico, mas nao ha jogo a decorrer rn
            create_ST_str(p, game_file_name, str_msg);
            snprintf(header, TCP_HEADER_SIZE_ST, "RSS FIN STATE_%06d.txt %03zu\n", plid, strlen(str_msg));

            write(fd, header, strlen(header));
            write(fd, str_msg, strlen(str_msg));
            
            return TRUE;   
        }
    }

    else { // jogo a decorrer

        snprintf(game_file_name, sizeof(game_file_name)-1, "./GAMES/GAME_%06d.txt", plid);
        
        create_ST_str(p, game_file_name, str_msg);
        snprintf(header, TCP_HEADER_SIZE_ST, "RSS ACT STATE_%06d.txt %03zu\n", plid, strlen(str_msg));

        write(fd, header, strlen(header));
        write(fd, str_msg, strlen(str_msg));
        return TRUE;   
    }
}
#include <cmds.h>

#include "aux.h"
#include "error_msgs.h"



int cmd_start(char input[BUFFER_SIZE], char sv_protocol[BUFFER_SIZE]) {
    int plid, time;
    char extra = '\0';

    if ((sscanf(input, "start %d %d%c", &plid, &time, &extra) != 2) || (extra != '\0')) {;
        printf("Input com argumentos invalidos...\n");
        printf("Input na forma: start PLID TIME\n");
        return FALSE;   
    }
    
    if (time > MAX_TIME) {
        printf("O tempo fornecido (%ds) não pode exceder 600s.\n", time); 
        return FALSE;
    }

    if (time < 0) {
        printf("Tempo não pode ser negativo...\n");
        return FALSE;
    }

    if (plid > MAX_6CHAR_NUMBER) {
        printf("O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", plid); 
        return FALSE;
    }

    if (plid < 0) {
        printf("O PlayerID fornecido não pode ser negativo.\n");
        return FALSE;
    }

    while((plid != player.PLID) && (player.PLID != NULL_PLID)) {
        // se o PLID nao nulo for diferente do plid no novo jogo entao...
        printf("Termine a sessão atual (PLID: %d).\n", player.PLID);
        if(!cmd_quit(sv_protocol)) {
            player.PLID = plid;
        }
    }

    // ------ ------ ------ ------ ------ ------ ------ ------ ------

    size_t n = snprintf(sv_protocol, BUFFER_SIZE, "SNG %d %d\n", plid, time);
    
    if ((n <= 0) || (n >= (size_t)BUFFER_SIZE)) {
        printf("Erro na tradução de protocolo. Tente outra vez!\n"); 
        return FALSE;
    }
    
    if (send_udp_msg(sv_protocol))       printf("[LOG] Sent: %s", sv_protocol);
    if (receive_udp_msg(sv_protocol))    printf("[LOG] Received: %s", sv_protocol);

    sv_protocol[strcspn(sv_protocol, "\n")] = '\0';
    if (strcasecmp(sv_protocol, "RSG OK") == 0) {
        printf("New game started (max %d sec)\n", time);   
    }
    else if (strcasecmp(sv_protocol, "RSG NOK") == 0){
        printf("ERRO, o jogo não começou, tente novamente e/ou termine o jogo atual");
    } 
    else printf("Pedido de inicio de jogo com argumentos invalidos.");

    player.GameTries = 1;
    player.PLID = plid;
    return TRUE;
}

void create_file(const char *file_name, const char *content) {
    char curdir[256];

    if (getcwd(curdir, sizeof(curdir)) == NULL) {
        perror("Erro ao obter o diretório atual");
        return;
    }
    
    char file_path[256 + 32];   // 32 -> (tamanho de /sv_responses/ + tamanho MAX de file_name)"
    
    snprintf(file_path, sizeof(file_path), "%s/sv_responses/%s", curdir, file_name);

    printf("[LOG] Saving %s\n", file_path);
    
    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("Erro ao criar o arquivo");
        return;
    }

    fprintf(file, "%s", content);
    
    fclose(file);
}

int cmd_st(char sv_protocol[BUFFER_SIZE]) {

    if (player.PLID == NULL_PLID) { printf("Erro: Comece um jogo!\n"); return FALSE; } 
    
    size_t n = snprintf(sv_protocol, BUFFER_SIZE, "STR %d\n", player.PLID);
    
    if ((n <= 0) || (n >= (size_t)BUFFER_SIZE)) {
        printf("Erro na tradução de protocolo. Tente outra vez!\n"); 
        return FALSE;
    }

    responseTCP res = two_way_tcp(sv_protocol, TCP_HEADER_SIZE_ST);

    if (!res.inited) { printf("Nenhuma partida encontrada para este jogador.\n"); return FALSE;}
    
    create_file(res.file_name, res.file_content);
    
    printf("Saved %s (%ld B) at /sv_responses\n", res.file_name, res.file_size);
    
    free(res.file_content);
    return TRUE;
}


int cmd_sb(char sv_protocol[BUFFER_SIZE]) {

    strcpy(sv_protocol, "SSB\n");

    responseTCP res = two_way_tcp(sv_protocol, TCP_HEADER_SIZE_SB);

    if (res.empty)   { printf("A scoreboard ainda está vazia.\n"); return TRUE; }
    if (!res.inited) { printf("Erro no display da scoreboard.\n"); return FALSE; }

    create_file(res.file_name, res.file_content);
    
    printf("Saved %s (%ld B) at /sv_responses\n", res.file_name, res.file_size);

    free(res.file_content);
    return TRUE;
}


int cmd_try(char input[BUFFER_SIZE], char sv_protocol[BUFFER_SIZE]) {
    
    char cor1, cor2, cor3, cor4, extra = '\0';

    if((sscanf(input, "try %c %c %c %c%c", &cor1, &cor2, &cor3, &cor4, &extra) != 4) || (extra != '\0')) {
        printf("Input com argumentos invalidos...\n");
        printf("Input na forma: try cor1 cor2 cor3 cor4\n");
        return FALSE;   
    }

    if (player.PLID == NULL_PLID) {
        printf("ERRO. Inicie o jogo.\n");
        return FALSE;
    }
    
    size_t n = snprintf(sv_protocol, BUFFER_SIZE, "TRY %d %c %c %c %c %d\n", player.PLID, cor1, cor2, cor3, cor4, player.GameTries);
    
    if((n <= 0) || (n >= (size_t)BUFFER_SIZE)) {
        printf("Erro na tradução de protocolo. Tente outra vez!\n");
        return FALSE;
    }
    
    if (send_udp_msg(sv_protocol))       printf("[LOG] Sent: %s", sv_protocol);
    if (receive_udp_msg(sv_protocol))    printf("[LOG] Received: %s", sv_protocol);
    
    if (strstr(sv_protocol, "RTR OK")) {
        int nT, nB, nW;
        
        char* sv_msg = &sv_protocol[7]; // obter a substring apos a resposta segundo o protocolgo (ex.: "RQT OK [sv_msg]")
        sscanf(sv_msg, "%d %d %d", &nT, &nB, &nW);

        if (nB == 4) {
            printf("\n!PARABÉNS ACERTOU! Jogo Terminado.\n");
            return TRUE;
        } 
        else {
            printf("Cores certas & Posições erradas: %d\n", nW);
            printf("Cores certas & Posições certas:  %d\n", nB);
            printf("Tentativas: %d\n", nT);
        }
    }
    else err_try_cmd(sv_protocol);
    player.GameTries++;
    return TRUE;
}

int cmd_debug(char input[BUFFER_SIZE], char sv_protocol[BUFFER_SIZE]) {

    int plid, time;
    char cor1, cor2, cor3, cor4, extra = '\0';

    if((sscanf(input, "debug %d %d %c %c %c %c%c",\
    &plid, &time, &cor1, &cor2, &cor3, &cor4, &extra) != 6) || (extra != '\0')) {
        
        printf("Input com argumentos invalidos...\n");
        printf("Input na forma: debug PLID TIME cor1 cor2 cor3 cor4\n");
        return FALSE;   
    }
    
    if(time > MAX_TIME) {
        printf("O tempo fornecido (%ds) não pode exceder 600s.\n", time); 
        return FALSE;
    }

    if (time < 0) {
        printf("Tempo não pode ser negativo...\n");
        return FALSE;
    }

    if (plid > MAX_6CHAR_NUMBER) {
        printf("O PlayerID fornecido (%d) não pode exceder 6 caracteres.\n", plid); 
        return FALSE;
    }

    if (plid < 0) {
        printf("O PlayerID fornecido não pode ser negativo.\n");
        return FALSE;
    }

    while((plid != player.PLID) && (player.PLID != NULL_PLID)) {
        // se o PLID nao nulo for diferente do plid no novo jogo entao...
        printf("Termine a sessão atual (PLID: %d).\n", player.PLID);
        if(!cmd_quit(sv_protocol)) {
            player.PLID = plid;
        }
    }



    // ------ ------ ------

    size_t n = snprintf(sv_protocol, BUFFER_SIZE, "DBG %d %d %c %c %c %c\n",\
    plid, time, cor1, cor2, cor3, cor4);
    
    if((n <= 0) || (n >= (size_t)BUFFER_SIZE)) {
        printf("Erro na tradução de protocolo. Tente outra vez!\n");
        return FALSE;
    }
    
    if (send_udp_msg(sv_protocol))       printf("[LOG] Sent: %s", sv_protocol);
    if (receive_udp_msg(sv_protocol))    printf("[LOG] Received: %s", sv_protocol);

    sv_protocol[strcspn(sv_protocol, "\n")] = '\0';
    if (strcasecmp(sv_protocol, "RDB OK") == 0) {
        printf("New debug game started (max %d sec)\n", time);    
    }
    else if (strcasecmp(sv_protocol, "RDB NOK") == 0) {
        printf("ERRO, o jogo de debug não começou, tente novamente e/ou termine o jogo atual");
    }
    else printf("Pedido de debug com argumentos invalidos.");

    player.GameTries = 1;
    player.PLID = plid;
    return TRUE;
}

int cmd_quit(char sv_protocol[BUFFER_SIZE]) { 
    
    if (player.PLID == NULL_PLID) {
        printf("Não há nenhum jogo a correr.");
        return TRUE;
    }

    int confirmacao = aux_confirmacao_saida();
    if (confirmacao == FALSE) return FALSE;
    
    size_t n = snprintf(sv_protocol, BUFFER_SIZE, "QUT %d\n", player.PLID);
    
    if((n <= 0) || (n >= (size_t)BUFFER_SIZE)) {
        printf("Impossivel de sair... Tente de novo.\n"); 
        return FALSE;
    }


    if (send_udp_msg(sv_protocol))       printf("[LOG] Sent: %s", sv_protocol);
    if (receive_udp_msg(sv_protocol))    printf("[LOG] Received: %s", sv_protocol);

    if (strstr(sv_protocol, "RQT OK")) {
        char* sv_msg = &sv_protocol[7]; // obter a substring apos a resposta segundo o protocolgo (ex.: "RQT OK [sv_msg]")
        printf("\nA solução do jogo era: %s", sv_msg);
    }
    else if (strstr(sv_protocol, "RQT NOK")) {
        printf("Não há nenhum jogo a correr com PLID %d.\n", player.PLID);
        return FALSE;
    }

    else {
        printf("Erro desconhecido.");
        printf("Erro em questão: %s\n", sv_protocol);
        return FALSE;
    }
    
    return TRUE;
}

void cmd_exit(char sv_protocol[BUFFER_SIZE]) { 
    
    int confirmacao = aux_confirmacao_saida();
    if (confirmacao == FALSE) return;
    
    if (player.PLID == NULL_PLID) { system("clear"); exit(EXIT_SUCCESS); }

    size_t n = snprintf(sv_protocol, BUFFER_SIZE, "QUT %d\n", player.PLID);
    
    if((n <= 0) || (n >= (size_t)BUFFER_SIZE)) {
        printf("Erro ao sair. Tente novamente\n"); 
        return;
    }

    if (send_udp_msg(sv_protocol))       printf("[LOG] Sent: %s", sv_protocol);
    if (receive_udp_msg(sv_protocol))    printf("[LOG] Received: %s", sv_protocol);

    freeaddrinfo(res);
    close(fd);
    system("clear");
    exit(EXIT_SUCCESS);
}


int aux_confirmacao_saida() {
    char response[MAX_RESPONSE];
    
    while (1) {
        printf("Tem a certeza que deseja sair? (sim/nao): ");
        if (fgets(response, MAX_RESPONSE, stdin) == NULL) {
            printf("Erro na entrada. Tente novamente.\n");
            continue;
        }
        
        // Remover o caractere de nova linha da resposta
        response[strcspn(response, "\n")] = '\0';
        
        if (strcasecmp(response, "s") == 0 || strcasecmp(response, "sim") == 0) {
            return TRUE; 
        } else if (strcasecmp(response, "n") == 0 || strcasecmp(response, "nao") == 0){
            return FALSE; 
        } else {
            printf("Resposta inválida. Por favor, digite 's/sim' para sim ou 'n/nao' para não.\n");
        }
    }
}

#include <protocols_responses.h>

int updt_score(Player *p) {
    if (p == NULL || p->gameStarted == FALSE) return -1;
      
    float time_score = 0, colors_score = 0;

    int cores_certas = p->tentativas[p->numTentativas - 1].nB;
    int cores_certas_lugar_errado = p->tentativas[p->numTentativas - 1].nW;

    //time_t tempo_total_jogo = p->timeOfEnd - p->timeOfStart;
    time_t remaining_time   = p->timeOfEnd - time(NULL);

    time_score = ((float)remaining_time / (float)MAX_TIME) * TIME_SCORE_WEIGHT; 

    colors_score = ((float)cores_certas / TRY_SEQ_SIZE) * COLORS_SCORE_WEIGHT;
    colors_score += ((float)cores_certas_lugar_errado / TRY_SEQ_SIZE) * HALF_RIGHT_COLORS_SCORE_WEIGHT;
    
    p->score = (int)(time_score + colors_score);

    return p->score;
}


void done_SNG(struct sockaddr_in Addr, int fd) {
    
    ssize_t n = sendto(fd, "RSG OK\n", strlen("RSG OK\n"), 0, (struct sockaddr*)&Addr, sizeof(Addr));
    if (n == -1) perror("sendto");
}


void err_SNG(char* err_msg, struct sockaddr_in Addr, int fd) {

    char buffer[MAX_SV_STD_RESPONSE];

    sprintf(buffer, "RSG %s\n", err_msg);

    ssize_t n = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&Addr, sizeof(Addr)); 
    if (n == -1) perror("sendto");
}

void done_DBG(struct sockaddr_in Addr, int fd) {

    ssize_t n = sendto(fd, "RDB OK\n", strlen("RDB OK\n"), 0, (struct sockaddr*)&Addr, sizeof(Addr));
    if (n == -1) perror("sendto");
}

void err_DBG(char* err_msg, struct sockaddr_in Addr, int fd) {
    char buffer[MAX_SV_STD_RESPONSE];

    sprintf(buffer, "RDB %s\n", err_msg);

    ssize_t n = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&Addr, sizeof(Addr)); 
    if (n == -1) perror("sendto");
}


void done_TRY(struct sockaddr_in Addr, int fd) {   
    char output[BUFFER_SIZE];
    Player* p = find_player_by_IP(Addr);

    updt_score(p);

    sprintf(output, "RTR OK %d %d %d\n", p->numTentativas, p->tentativas[p->numTentativas-1].nB, p->tentativas[p->numTentativas-1].nW);
    ssize_t n = sendto(fd, output, strlen(output), 0, (struct sockaddr*)&Addr, sizeof(Addr));
    if (n == -1) perror("sendto");
}


void err_TRY(char* err_msg, struct sockaddr_in Addr, int fd) {

    char buffer[MAX_SV_STD_RESPONSE];

    sprintf(buffer, "RTR %s\n", err_msg);

    ssize_t n = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&Addr, sizeof(Addr)); 
    if (n == -1) perror("sendto");
}


void err_QUT(char* err_msg, struct sockaddr_in Addr, int fd) {
    
    char buffer[MAX_SV_STD_RESPONSE];

    sprintf(buffer, "RQT %s\n", err_msg);

    ssize_t n = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&Addr, sizeof(Addr)); 
    if (n == -1) perror("sendto");
}

void done_QUT(char* solution, struct sockaddr_in Addr, int fd) {

    char output[17];

    snprintf(output, sizeof(output)-1, "RQT OK %c %c %c %c\n", solution[0], solution[1], solution[2], solution[3]);

    ssize_t n = sendto(fd, output, strlen(output), 0, (struct sockaddr*)&Addr, sizeof(Addr));
    if (n == -1) perror("sendto");
}

void err_SSB(char* err_msg, int fd) {
    
    char buffer[MAX_SV_STD_RESPONSE];

    sprintf(buffer, "RQT %s\n", err_msg);

    ssize_t n = write(fd, buffer, strlen(buffer)); 
    if (n == -1) perror("write");
}

void done_SSB(int fd) {
    
    ssize_t n = write(fd, "RQT OK\n", strlen("RSB OK\n"));
    if (n == -1) perror("write");
}

void err_STR(char* err_msg, int fd) {
    
    char buffer[MAX_SV_STD_RESPONSE];

    sprintf(buffer, "RST %s\n", err_msg);

    ssize_t n = write(fd, buffer, strlen(buffer)); 
    if (n == -1) perror("write");
}

void done_STR(int fd) {
    
    ssize_t n = write(fd, "RST ACT\n", strlen("RST ACT\n"));
    if (n == -1) perror("write");
}
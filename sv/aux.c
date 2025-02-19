#include <aux.h>

void get_svIP() {
    char hostname[256];
    struct hostent *host_;
    struct in_addr **addr_list;

    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return;
    }

    host_ = gethostbyname(hostname);
    if (host_ == NULL) {
        perror("gethostbyname");
        return;
    }

    addr_list = (struct in_addr **)host_->h_addr_list;
    printf("\nHosted at: %s:%d\n\n", inet_ntoa(*addr_list[0]), PORT);
}


void get_cmd(char input[BUFFER_SIZE], char cmd[CMD_SIZE+1]) {
    sscanf(input, "%3s ", cmd);
}

void init_udp(int* udp_fd) {
    struct addrinfo hints, *res_udp;
    int errcode;

    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;

    if((errcode = getaddrinfo(NULL, MYPORT, &hints, &res_udp))!=0) {
        printf("Erro no getadrrinfo\n");
        close(*udp_fd);
        exit(1);
    }

    *udp_fd = socket(res_udp->ai_family,res_udp->ai_socktype,res_udp->ai_protocol);
    if(*udp_fd==-1) {
        printf("Erro a criar socket\n");
        close(*udp_fd);
        exit(1);
    }

    if(bind(*udp_fd,res_udp->ai_addr,res_udp->ai_addrlen)==-1) {
        perror("Erro ao fazer bind no socket");
        close(*udp_fd);
        exit(1);
    }
}


void init_tcp(int* tcp_fd) {
    struct addrinfo hints, *res_tcp;
    int errcode;

    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;


    if((errcode = getaddrinfo(NULL, MYPORT, &hints, &res_tcp))!=0) {
        printf("Erro no getadrrinfo\n");
        exit(1);
    }

    *tcp_fd = socket(res_tcp->ai_family,res_tcp->ai_socktype,res_tcp->ai_protocol);
    
    if(*tcp_fd == -1) {
        printf("Erro a criar socket\n");
        exit(1);
    }

    if (bind(*tcp_fd, res_tcp->ai_addr,res_tcp->ai_addrlen) < 0) {
        perror("TCP bind failed");
        close(*tcp_fd);
        exit(1);
    }


    if (listen(*tcp_fd, MAX_TCP_CONNECTIONS) < 0) {
        perror("TCP listen failed");
        close(*tcp_fd);
        exit(1);
    }
}

void init_udp_talk(int fd, char client_protocol[BUFFER_SIZE], struct sockaddr_in addr) {
    char host[NI_MAXHOST], service[NI_MAXSERV];

    memset(client_protocol, '\0', BUFFER_SIZE);
    unsigned int addrlen = sizeof(addr);
    
    ssize_t n = recvfrom(fd, client_protocol, BUFFER_SIZE-1, 0, (struct sockaddr *)&addr, &addrlen);
    
    if (n < 0) perror("Erro UDP");
    
    if(strlen(client_protocol) > 0) client_protocol[n] = '\0';
    
    int errcode = getnameinfo( (struct sockaddr *) &addr, addrlen, host, sizeof(host), service, sizeof(service), 0);
    if(errcode != 0) printf("error");
    
    printf("[LOG] <UDP> Received: %s[LOG] <UDP> From: %s:%s\n", client_protocol, host, service);
}




void imprimir_tentativa(const Tentativa* tentativa) {
    if (!tentativa) {
        printf("Tentativa inválida.\n");
        return;
    }

    printf("Tentativa #%d\n", tentativa->num_tentativa);
    printf("Sequência: ");
    for (int i = 0; i < 4; i++) {
        printf("%c ", tentativa->sequencia[i]);
    }
    printf("\n");
    printf("Peças no lugar certo (nB): %d\n", tentativa->nB);
    printf("Peças certas no lugar errado (nW): %d\n", tentativa->nW);
    printf("----------------------------\n");
}



int FindLastGame(char *plid, char *fname) {
    struct dirent **filelist;
    int n_entries, found;
    char dirname[20];

    sprintf(dirname, "GAMES/%s/", plid);
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    found = 0;

    if (n_entries <= 0)
        return 0;
    else {
        while (n_entries--) {
            if (filelist[n_entries]->d_name[0] != '.' && !found) {
                sprintf(fname, "GAMES/%s/%s", plid, filelist[n_entries]->d_name);
                found = 1;
            }
            free(filelist[n_entries]);
        }
        free(filelist);
    }

    return found;
}


int FindTopScores(SCORELIST *list) {
    struct dirent **filelist;
    int n_entries, ifile;
    char fname[300];
    FILE *fp;
    char mode[8];

    n_entries = scandir("SCORES/", &filelist, 0, alphasort);
    if (n_entries <= 0)
        return 0;
    else {
        ifile = 0;
        while (n_entries--) {
            if (filelist[n_entries]->d_name[0] != '.' && ifile < 10) {
                sprintf(fname, "SCORES/%s", filelist[n_entries]->d_name);
                fp = fopen(fname, "r");
                if (fp != NULL) {
                    fscanf(fp, "%d %s %s %d %s",
                           &list->score[ifile],
                           list->PLID[ifile],
                           list->col_code[ifile],
                           &list->notries[ifile],
                           mode);

                    if (!strcmp(mode, "PLAY")) {
                        list->mode[ifile] = MODEPLAY;
                    }
                    if (!strcmp(mode, "DEBUG")){
                        list->mode[ifile] = MODEDEBUG;
                    }

                    fclose(fp);
                    ++ifile;
                }
            }
            free(filelist[n_entries]);
        }
        free(filelist);
    }

    list->nscores = ifile;
    return ifile;
}

void inserirEspacos(const char *original, char *modificada) {
    int i, j = 0;
    int len = strlen(original);

    for (i = 0; i < len; i++) {
        modificada[j++] = original[i];
        if (i < len - 1) {
            modificada[j++] = ' ';
        }
    }
    modificada[j] = '\0'; 
}

void createDir(int plid_int) {
    char fullPath[300]; // Buffer para armazenar o caminho completo
    struct stat st = {0};

    char* basePath = "./GAMES";

    char plid[7];
    sprintf(plid, "%d", plid_int);

    snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, plid);

    if (stat(fullPath, &st) == -1) {
        if (mkdir(fullPath, 0700) == 0) {
            printf("[LOG] Diretório '%s' criado com sucesso!\n", fullPath);
        } else {
            perror("Erro ao criar o diretório");
        }
    } else {
        printf("[LOG] Diretório '%s' já existe.\n", fullPath);
    }
}


int writeHeader(Player *player, char mode) {
    char date[11]; // Para armazenar a data no formato YYYY-MM-DD
    char time[9];  // Para armazenar a hora no formato HH:MM:SS


    struct tm *tm_start = localtime(&player->timeOfStart);
    if (strftime(date, sizeof(date), "%Y-%m-%d", tm_start) == 0 ||
        strftime(time, sizeof(time), "%H:%M:%S", tm_start) == 0) {
        return -1; 
    }

    char filename[64];
    snprintf(filename, sizeof(filename), "./GAMES/GAME_%06d.txt", player->PLID);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    char buffer[64];
    unsigned int len = snprintf(buffer, (sizeof(buffer)-1), "%06d %c %c%c%c%c %d %s %s %ld\n",
                       player->PLID, mode,
                       player->solution[0], player->solution[1], player->solution[2], player->solution[3],
                       (int)(player->timeOfEnd - player->timeOfStart),
                       date, time, player->timeOfStart);

    if (len >= sizeof(buffer)) {
        close(fd);
        return -1; 
    }

    ssize_t bytes_written = write(fd, buffer, len);
    if (bytes_written != len) {
        perror("Erro ao escrever no arquivo");
        close(fd);
        return -1;
    }

    printf("[LOG] Header done %s\n", filename);

    close(fd);
    return TRUE;
}

int writeTry(Tentativa* t, Player* p) {

    char filename[64];
    snprintf(filename, sizeof(filename), "./GAMES/GAME_%06d.txt", p->PLID);

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    char buffer[32];
    memset(buffer, '\0', sizeof(buffer));

    unsigned int len = snprintf(buffer, (sizeof(buffer)-1), "T: %c%c%c%c %d %d %03ld\n",\
    t->sequencia[0], t->sequencia[1], t->sequencia[2], t->sequencia[3], t->nB, t->nW,\
    (time(NULL) - p->timeOfStart));

    if (len >= sizeof(buffer)) {
        close(fd);
        return -1; // Buffer overflow
    }

    // Escrever no arquivo
    ssize_t bytes_written = write(fd, buffer, len);
    if (bytes_written != len) {
        perror("Erro ao escrever no arquivo");
        close(fd);
        return -1;
    }

    printf("[LOG] Try written at %s\n", filename);
    
    close(fd);
    return TRUE;
}


struct tm *writeFooter_and_saveFile(Player *p, char win_or_loss) {
    char tmp_new_filename[64], new_filename[64], date_hour[20];
    char line[32];

    char filename[64];
    snprintf(filename, sizeof(filename), "./GAMES/GAME_%06d.txt", p->PLID);

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    time_t time_ = time(NULL);

    struct tm *time_of_end = localtime(&time_);

    if(strftime(date_hour, 20, "%Y-%m-%d %H:%M:%S", time_of_end) == 0) return NULL;

    if(strftime(tmp_new_filename, sizeof(tmp_new_filename), "%Y%m%d_%H%M%S", time_of_end) == 0) return NULL;


    unsigned int len = snprintf(line, (sizeof(line)-1), "%s %03ld", date_hour, (time_ - p->timeOfStart));
    unsigned int len_ = snprintf(new_filename, (sizeof(new_filename)-1), "%s_%c.txt", tmp_new_filename ,win_or_loss);

    if ((len >= sizeof(line)) || (len_ >= sizeof(new_filename))) {
        close(fd);
        return NULL;; // Buffer overflow
    }

    // Escrever no arquivo
    ssize_t bytes_written = write(fd, line, len);
    if (bytes_written != len) {
        perror("Erro ao escrever no arquivo");
        close(fd);
        return NULL;
    }

    printf("[LOG] Try written at ./GAMES/%s\n", filename);
    close(fd);

    char str_plid[7];
    sprintf(str_plid, "%06d", p->PLID);

    rename_and_move_file(filename, new_filename, str_plid);
    return time_of_end;
}

void rename_and_move_file(char *file_path, char *new_name, char *plid) {
    char destination_folder[64];
    char new_file_path[128];

    // pasta destino
    snprintf(destination_folder, sizeof(destination_folder), "./GAMES/%s", plid);
    snprintf(new_file_path, sizeof(new_file_path), "%s/%s", destination_folder, new_name);

    // rename e mv file
    if (rename(file_path, new_file_path) == 0) {
        printf("File moved to: %s\n", new_file_path);
    } else {
        perror("Error moving file");
    }
}



void save_score(Player *p, struct tm *time_of_end) {

    char filename[64], line[32], date_hour[20], mode[6];

    if(strftime(date_hour, 20, "%Y%m%d_%H%M%S", time_of_end) == 0) return;

    snprintf(filename, (sizeof(filename)-1), "./SCORES/%03d_%06d_%s.txt",\
            p->score, p->PLID, date_hour);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return;
    }


    if      (p->mode == MODEPLAY)   strcpy(mode, "PLAY");
    else if (p->mode == MODEDEBUG)  strcpy(mode, "DEBUG");
    
    else { printf("<%d>: NO MODE DEFINED FOR PLAYER!", p->PLID); close(fd); return; }

    // confirmar o numTentativas TODO
    unsigned int len = snprintf(line, (sizeof(line)-1), "%03d %06d %4s %d %s\n",\
    p->score, p->PLID, p->tentativas[(p->numTentativas-1)].sequencia, p->numTentativas, mode);

    ssize_t bytes_written = write(fd, line, len);
    if (bytes_written != len) {
        perror("Erro ao escrever no arquivo");
        close(fd);
        return;
    }

    printf("[LOG] Header done %s\n", filename);

    close(fd);
}



void end_game(Player *p, char win_or_loss) {
    
    struct tm *time_of_end = writeFooter_and_saveFile(p, win_or_loss);
    if(time_of_end == NULL) return;

    if(win_or_loss == WIN) save_score(p, time_of_end);

    p->gameStarted = FALSE;
    p->numTentativas = 0;
}


int tentativaJaExiste(char* sequencia, Player* player_atual) {
    for (int i = 0; i < player_atual->numTentativas; i++) {
        //printf("Sequencia do player: '%s' (len: %zu)\n", player_atual->tentativas[i].sequencia, strlen(player_atual->tentativas[i].sequencia));
        //printf("Sequencia atual: '%s' (len: %zu)\n", sequencia, strlen(sequencia));

        if (strcmp(player_atual->tentativas[i].sequencia, sequencia) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void print_player_list(Player_list* head) {
    // Verifica se a lista está vazia
    if (head == NULL) {
        printf("A lista de players está vazia.\n");
        return;
    }

    printf("Lista de Players:\n");
    int player_count = 1;
    
    // Percorre cada nó na lista
    while (head != NULL) {
        Player* p = &head->player;  // Acessa o jogador atual
        printf("Player %d:\n", player_count++);
        printf("  PLID: %d\n", p->PLID);
        printf("  Game Started: %s\n", p->gameStarted ? "Yes" : "No");
        printf("  Time of Start: %ld\n", p->timeOfStart);
        printf("  Time of End: %ld\n", p->timeOfEnd);
        printf("  Solution: %s\n", p->solution); // Mostra apenas 4 caracteres da solução
        printf("  Number of Attempts: %d\n", p->numTentativas);
        printf("  Score: %d\n", p->score);
        printf("  Mode: %d\n", p->mode);
        
        // Imprime as tentativas
        if (p->numTentativas > 0) {
            printf("  Attempts:\n");
            for (int i = 0; i < p->numTentativas; i++) {
                Tentativa* t = &p->tentativas[i];
                int j = i+1;
                printf("    Attempt %d: %s (nB: %d, nW: %d)\n",
                       j, t->sequencia, t->nB, t->nW);
            }
        } else {
            printf("  No attempts made yet.\n");
        }

        // Passa para o próximo nó na lista
        head = head->next;
    }
}


int sb_file_count() {
    const char *filename = "SB_file_num.txt";
    FILE *file;
    int counter = 0;


    file = fopen(filename, "r");
    if (file == NULL) {
        file = fopen(filename, "w");
        if (file == NULL) {
            perror("Erro ao criar o arquivo");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%d", counter);
        fclose(file);
    } else {
        if (fscanf(file, "%d", &counter) != 1) {
            counter = 0; 
        }
        fclose(file);
    }

    counter++;

    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d", counter);
    fclose(file);

    return counter;
}


int create_ST_str(Player *p, char filename[128], char* buffer) {

    if(p->gameStarted == FALSE) { // caso jogo em historico

        // Header
        char date_hour[20];
        struct tm *tm0 = localtime(&p->timeOfStart);
        strftime(date_hour, sizeof(date_hour), "%Y-%m-%d %H:%M:%S", tm0);

        sprintf(buffer,
        "\tLast finalized game for player %06d\n"
        "Game initiated: %s with %d seconds to be completed\n\n"
        "\t--- Transactions found: ---\n\n",
        p->PLID, date_hour, (int)(p->timeOfEnd-p->timeOfStart));

        // -----

        char line[64], try[5];
        int nB, nW, time_elapsed, try_n=0;
        FILE *file = fopen(filename, "r");

        while(fgets(line, sizeof(line), file) != NULL) {
            if(line[0] != 'T') continue;
            //T: RRRR 0 0 005
            sscanf(line, "T: %4s %d %d %03d", try, &nB, &nW, &time_elapsed);

            try_n++;

            char try_line[40] = {0};

            snprintf(try_line, sizeof(try_line), "Trial: %s, nB: %d, nW: %d at %3ds\n",
                    try, nB, nW, time_elapsed);
            strcat(buffer, try_line);
        }

        char last_line[32] = {0};

        sprintf(last_line, "\n\t-- Game Terminated  -- "); 

        strcat(buffer, last_line);
        return TRUE;
    }
    else {          // caso jogo a decorrer
        // Header
        char date_hour[20];
        struct tm *tm0 = localtime(&p->timeOfStart);
        strftime(date_hour, sizeof(date_hour), "%Y-%m-%d %H:%M:%S", tm0);

        sprintf(buffer,
        "\tActive game found for player %06d\n"
        "Game initiated: %s with %d seconds to be completed\n"
        "\t--- Transactions found: %d ---\n\n",
        p->PLID, date_hour, (int)(p->timeOfEnd-p->timeOfStart), p->numTentativas);

        // -----

        char line[64], try[5];
        int nB, nW, time_elapsed, try_n=0;
        FILE *file = fopen(filename, "r");

        while(fgets(line, sizeof(line), file) != NULL) {
            if(line[0] != 'T') continue;
            sscanf(line, "T: %4s %d %d %03d", try, &nB, &nW, &time_elapsed);

            try_n++;

            char try_line[40] = {0};

            snprintf(try_line, sizeof(try_line), "Trial: %s, nB: %d, nW: %d at %3ds\n",
                    try, nB, nW, time_elapsed);
            strcat(buffer, try_line);
        }

        char last_line[60] = {0};

        sprintf(last_line, "\n\t-- %3d seconds remaining to be completed -- ",
        (int)(p->timeOfEnd - time(NULL)));

        strcat(buffer, last_line);
        return TRUE;
    }
}
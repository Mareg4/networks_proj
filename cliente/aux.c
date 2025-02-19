#include <aux.h>
#include "cmds.h"
#include <errno.h>
#include <unistd.h>


void init_player(Player* p) {
    p->PLID = NULL_PLID;
    p->GameTries = 1;
}


void read_input(char input[BUFFER_SIZE]) {

    printf("\n> ");
    fflush(stdout);

    if (!fgets(input, BUFFER_SIZE, stdin)) {
        perror("Erro ao ler a entrada.");
        strcpy(input, "\0");
        return;
    }
    input[strcspn(input, "\n")] = '\0';
    
    printf("[LOG] Input: %s\n", input);
}


void get_cmd(char input[BUFFER_SIZE], char cmd[CMD_SIZE]) {
    memset(cmd, '\0', 16);
    sscanf(input, "%s ", cmd);
}


char* make_protocol_msg(char* protocol_cmd, char* args) {

    if (args == NULL) { printf("Mensagem sem argumentos..."); return NULL; }

    size_t msg_len = strlen(protocol_cmd) + strlen(args) + 2; // +1 \n +1 \0

    if (msg_len > (size_t)BUFFER_SIZE) { printf("Argumentos da mensagen muito longos... %ld/%d", msg_len-2, BUFFER_SIZE-2); return NULL; } // TESTAR 
    
    char* msg = (char *)malloc((msg_len)*sizeof(char));
    
    strcpy(msg, protocol_cmd);
    strcat(msg, args);
    strcat(msg, "\n");

    return msg;
}

void aux_connect(char* IP, char* PORT, char buffer[BUFFER_SIZE]) {
    int trys = 0;
    while(!try_connect(IP, PORT, buffer) && (trys < MAX_TRYS)) {
        trys++;
        printf("[LOG] (%d) Connection Failed. Trying again.\n", trys);
        sleep(1);
    }
    if (trys == MAX_TRYS) {
        printf("[ERROR] Maximum connection attempts reached. Exiting.\n");
        exit(EXIT_FAILURE);
    } else { printf("\nCONNECTED!\n"); }
}

int try_connect(char* IP, char* PORT, char buffer[BUFFER_SIZE]) {
    int errcode;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        printf("Failed socket creation.\n");
        return FALSE;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(IP, PORT, &hints, &res);
    if (errcode != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errcode));
        return FALSE;
    }
     
    struct timeval timeout;
    timeout.tv_sec = 1;  // Timeout de 1 segundo
    timeout.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        return FALSE;
    }
    
    if (sendto(fd, "teste\n", BUFFER_SIZE, 0, res->ai_addr, res->ai_addrlen) == -1) {
        perror("sendto failed");
        return FALSE;
    }

    if (recvfrom(fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&addr, &addrlen) == -1) {
        printf("\n");
        return FALSE;
    }
    return TRUE;
}

int send_udp_msg(char sv_protocol[BUFFER_SIZE]) {
    
    int trys = 0;
    while(!(sendto(fd, sv_protocol, strcspn(sv_protocol, "\n")+1, 0, res->ai_addr, res->ai_addrlen) > 0)\
        && (trys < MAX_TRYS)) {
        printf("(%d) Falha a mandar mensagem ao servidor. Tentando novamente...", ++trys);
        sleep(1);
    }

    if(trys < MAX_TRYS) return TRUE;
    
    else { 
        printf("Falha de conexção com o servidor.");
        exit(EXIT_FAILURE); 
    }
}


int receive_udp_msg(char sv_protocol[BUFFER_SIZE]) { 

    ssize_t n = recvfrom(fd, sv_protocol, BUFFER_SIZE-1, 0, (struct sockaddr *)&addr, &addrlen);
    int trys = 0;
    
    if (n <= 0) {
        while (trys < 2) {
        
            n = recvfrom(fd, sv_protocol, BUFFER_SIZE-1, 0, (struct sockaddr *)&addr, &addrlen);
            printf("(%d) Falha a receber mensagem ao servidor. Tentando novamente.\n", ++trys);
            sleep(1);
            
            if (n > 0) {

                if ((n+1) > BUFFER_SIZE) {return FALSE;}    // verificar se nao excede o buffer size
                sv_protocol[n] = '\0';
                return TRUE; 
            }
        }
        printf("Falha de conexão com o servidor.\n");
        exit(EXIT_FAILURE);
    }

    if ((n+1) > BUFFER_SIZE) {return FALSE;}        // verificar se nao excede o buffer size

    sv_protocol[n] = '\0';
    return TRUE;
}


int connect_tcp() {

    int errcode;
    struct addrinfo hints_tcp,*res_tcp;

    int fd_tcp = socket(AF_INET, SOCK_STREAM, 0); //TCP socket
    if (fd_tcp == -1) { printf("Erro a criar socket...\n"); return -1; }
    
    memset(&hints_tcp,0,sizeof hints_tcp);
    hints_tcp.ai_family=AF_INET; //IPv4
    hints_tcp.ai_socktype=SOCK_STREAM; //TCP socket

    errcode = getaddrinfo(IP, PORT, &hints_tcp, &res_tcp);
    if(errcode!=0) return -1;;
        
    ssize_t n = connect(fd_tcp, res_tcp->ai_addr, res_tcp->ai_addrlen);
    if (n == -1) { printf("Erro a conectar a socket.\n"); return -1; }

    freeaddrinfo(res_tcp);
    return fd_tcp;
}

responseTCP two_way_tcp(char sv_protocol[BUFFER_SIZE], int header_size) {
    
    int fd_tcp = connect_tcp();
    if (fd_tcp == -1) { printf("Erro a estabelecer conexão TCP... Reinicie o jogo."); exit(1); }

    
    if(!send_tcp_msg(sv_protocol, fd_tcp)) { printf("Erro a enviar msg tcp... Reinicie o jogo.\n"); exit(1); }
    
    
    responseTCP res = receive_tcp_msg(sv_protocol, header_size, fd_tcp);

    
    close(fd_tcp);
    return res;
}

int send_tcp_msg(char sv_protocol[BUFFER_SIZE], int fd_tcp) {

    if (write(fd_tcp, sv_protocol, strcspn(sv_protocol, "\n")+1) == -1) return FALSE; 
    
    printf("[LOG] sent: %s", sv_protocol); 
    return TRUE;
}


responseTCP receive_tcp_msg(char sv_protocol[BUFFER_SIZE], unsigned int header_size, int fd_tcp) {

    char sv_log[SV_LOG_MAX_SIZE], _[4];
    size_t n;
    
    responseTCP res;
    res.inited = FALSE;
    res.empty = FALSE;

    memset(sv_protocol, '\0', BUFFER_SIZE);

    n = simple_read_tcp(sv_protocol, header_size, fd_tcp);      // obter header
    printf("[LOG] Received Header tcp: %s", sv_protocol);
    
    if (n != (size_t)header_size) { printf("[LOG] Header != OK...\n");}
    
    sscanf(sv_protocol, "%s %s %s %zu", _, sv_log, res.file_name, &res.file_size);

    // caso o servidor deh uma resposta positiva
    if( (strstr(sv_log, "OK")   && !strstr(sv_log, "NOK")) ||\
        (strstr(sv_log, "FIN")) || (strstr(sv_log, "ACT")) ) {
        
        res.file_content = (char *) malloc((res.file_size+1) * sizeof(char));
        
        n = simple_read_tcp(res.file_content, res.file_size, fd_tcp);
        
        if (n != res.file_size) { printf("Erro a receber msg tcp: %ld/%zu ... Reinicie o jogo.\n", n, res.file_size); return res; }
        res.file_content[n] = '\0';
        
        printf("[LOG] Received File tcp. File data:\n%s\n", res.file_content);
        
        res.inited = TRUE;
        res.empty  = FALSE;
    }

    else if (strstr(sv_log, "EMPTY")) res.empty = TRUE;

    return res;
}



ssize_t simple_read_tcp(char* buffer_tcp, size_t n_bytes, int fd_tcp) {
    size_t total_read = 0;
    int c=0;

    while (total_read < n_bytes) {
        ssize_t bytes_read = read(fd_tcp, buffer_tcp + total_read, n_bytes - total_read);

        printf("[LOG] <TCP>: %zu/%zuB received\n", bytes_read, n_bytes);
        
        if (bytes_read > 0) total_read += bytes_read;
               
        else if (bytes_read == 0) { 
            c++; 
            sleep(2);
            if(c > TRY_AGAIN) return total_read;
            continue; 
        } 
        else if (errno == EINTR) continue;  
        else return -1;   
    }
    return total_read;
}


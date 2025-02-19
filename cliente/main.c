#include <main.h>

#include "cmds.h"
#include "aux.h"

char* IP; 
char* PORT;
char buffer[BUFFER_SIZE];
Player player;

/*-----------------------------------------MAIN-----------------------------------------------*/

void parse_args(int argc, char *argv[]) {

    //valores padrão
    IP = LOCALHOST; 
    PORT = DEFAULT_PORT; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            IP = argv[++i];
        } 
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            PORT = argv[++i];
        } 
        else {
            fprintf(stderr, "Uso: ./player [-n GSIP] [-p GSport]\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("GS at %s:%s\n\n", IP, PORT);
}

// funcao para lidar com o CTRL-C
void sigint_handler(int sig) {
    (void)sig; // CRINGE
    
    char sv_protocol[128];
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


int main(int argc, char *argv[]) {

    struct sigaction sa;
    sa.sa_handler = sigint_handler; 
    sa.sa_flags = 0;               
    sigemptyset(&sa.sa_mask);       
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    parse_args(argc, argv);
    char input[BUFFER_SIZE];
    aux_connect(IP, PORT, buffer);
    init_player(&player);
    
    while(TRUE) {
        memset(input,  '\0', BUFFER_SIZE);
        memset(buffer, '\0', BUFFER_SIZE);

        read_input(input);

        if(strcmp(input, "clear") == 0) { system("clear"); continue; }

        char cmd[CMD_SIZE];
        get_cmd(input, cmd);
 
        if      (strcmp(cmd, "start") == 0)                                     cmd_start(input, buffer);
        else if (strcmp(cmd, "debug") == 0)                                     cmd_debug(input, buffer);
        else if (strcmp(cmd, "try") == 0)                                       cmd_try(input, buffer);
        else if ((strcmp(cmd, "st") == 0) || (strcmp(cmd, "show_trials") == 0)) cmd_st(buffer);
        else if ((strcmp(cmd, "sb") == 0) || (strcmp(cmd, "scoreboard") == 0))  cmd_sb(buffer);
        else if (strcmp(cmd, "quit") == 0)                                      cmd_quit(buffer);
        else if (strcmp(cmd, "exit") == 0)                                      cmd_exit(buffer);
        else    printf("Insira um comando válido.\n");
    }
    freeaddrinfo(res);
    close(fd);
    return EXIT_FAILURE;
}

#include <main.h>

int v_mode = FALSE;
int PORT = DEFAULT_PORT;



void parse_args(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                PORT = atoi(argv[i + 1]);
                if (PORT <= 0) {
                    fprintf(stderr, "Error: Invalid port number.\n");
                    exit(EXIT_FAILURE);
                }
                i++;
            } else {
                fprintf(stderr, "Error: Missing value for -p option.\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-v") == 0) {
            v_mode = TRUE;
        } else {
            fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
}



int main(int argc, char *argv[]) {
    
    parse_args(argc, argv);
    get_svIP();

    char in_str[BUFFER_SIZE], client_protocol[BUFFER_SIZE];
    int errcode, udp_fd, tcp_fd; // out_fds
    fd_set inputs, testfds;
    ssize_t n = 1;

    struct timeval timeout;
    char host[NI_MAXHOST], service[NI_MAXSERV];

    init_udp(&udp_fd);
    init_tcp(&tcp_fd);

    // ------------- MAIN ----------------------------------

    while(1) {
        
        FD_ZERO(&inputs);           // Clear input mask
        FD_SET(0,&inputs);          // Set standard input channel on
        FD_SET(udp_fd,&inputs);     // Set UDP channel on
        FD_SET(tcp_fd, &inputs);    // Set TCP channel on

        testfds = inputs;           // Reload mask
        
        memset((void *)&timeout, 0, sizeof(timeout));
        timeout.tv_sec = SELECT_TIMEOUT_SEC;

        int max_fd = tcp_fd > udp_fd ? tcp_fd : udp_fd;
     
        switch(select(max_fd+1, &testfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *) &timeout)) {

            case 0:
                printf("\n... WAITING ...\n\n");
                print_player_list(head);    // Lista de players ate ao momento (debug)
                break;

            case -1:
                perror("select");
                exit(1);

            default:

                // verifica STDIN
                if(FD_ISSET(0, &testfds)) {       
                    fgets(in_str, BUFFER_SIZE, stdin);
                    printf("---Input at keyboard: %s\n",in_str);
                    if (strcmp(in_str, "clean\n") == 0) { eliminate_all_players(); printf("Servidor Reset.\n"); }// limpar servidor
                }

                // verifica UDP
                if(FD_ISSET(udp_fd, &testfds)) {

                    memset(client_protocol, '\0', BUFFER_SIZE);
                    addrlen = sizeof(addr);
                    
                    n = recvfrom(udp_fd, client_protocol, BUFFER_SIZE-1, 0, (struct sockaddr *)&addr, &addrlen);
                    
                    if (n < 0) perror("Erro UDP");
                    
                    if(strlen(client_protocol) > 0) client_protocol[n] = '\0';
                    
                    errcode = getnameinfo( (struct sockaddr *) &addr, addrlen, host, sizeof(host), service, sizeof(service), 0);
                    if(errcode != 0) printf("error");
                    
                    printf("[LOG] <UDP> Received: %s[LOG] <UDP> From: %s:%s\n", client_protocol, host, service);

                    // --------------------------------------------

                    char cmd[CMD_SIZE+1];
                    get_cmd(client_protocol, cmd);

                    if      (strcmp(cmd, "SNG") == 0)   SNG_protocol(client_protocol, addr, udp_fd);
                    else if (strcmp(cmd, "DBG") == 0)   DBG_protocol(client_protocol, addr, udp_fd);
                    else if (strcmp(cmd, "TRY") == 0)   TRY_protocol(client_protocol, addr, udp_fd);
                    else if (strcmp(cmd, "QUT") == 0)   QUT_protocol(client_protocol, addr, udp_fd);

                    // --------------------------------------------

                    else {
                        n = sendto(udp_fd, "ERR\n", strlen("ERR\n") , 0, (struct sockaddr *)&addr, addrlen); // XXX 
                        printf("[LOG] <%s>: Cmd desconhecido.", host);
                        if (n == -1) { perror("sendto"); exit(1); }
                    }
                }

                // verifica TCP
                if(FD_ISSET(tcp_fd, &testfds)) {

                    memset(client_protocol, '\0', BUFFER_SIZE);
                    int new_fd;
                    addrlen = sizeof(addr);
                    
                    if((new_fd = accept(tcp_fd, (struct sockaddr*)&addr, &addrlen))==-1) { perror("Accept error"); exit(1);}
                    
                    errcode = getnameinfo( (struct sockaddr *) &addr, addrlen, host, sizeof(host), service, sizeof(service), 0);
                    if(errcode != 0) printf("error");
                    
                    n = read(new_fd, client_protocol, BUFFER_SIZE);
                    if(n == -1) { perror("Read error"); exit(1); }

                    printf("[LOG] <TCP> Received: %s[LOG] <TCP> From: %s:%s\n", client_protocol, host, service);

                    // -------------------------------------------- 

                    char cmd[CMD_SIZE+1];
                    get_cmd(client_protocol, cmd);

                    if      (strcmp(cmd, "SSB") == 0)   SSB_protocol(new_fd);
                    else if (strcmp(cmd, "STR") == 0)   STR_protocol(client_protocol, new_fd);

                    // --------------------------------------------

                    else {
                        n = write(new_fd, "ERR\n", strlen("ERR\n")); // XXX 
                        printf("[LOG] <%s>: Cmd desconhecido.", host);
                        if (n == -1) { perror("sendto"); exit(1); }
                    } 
                    close(new_fd);
                }
        }
    }
    return 0;
}

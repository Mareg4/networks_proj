#include <error_msgs.h>

void err_try_cmd(const char sv_protocol[BUFFER_SIZE]) {
    char cor1, cor2, cor3, cor4;
    
    if (strstr(sv_protocol, "DUP")) {
        player.GameTries--;
        printf("Erro: DUP (Duplicate Guess) - A chave secreta repetiu uma tentativa anterior. O número de tentativas não foi incrementado.\n");
    } 
    else if (strstr(sv_protocol, "INV")) {
        printf("Erro: INV (Invalid Trial) - O número da tentativa (nT) não é o esperado ou a chave secreta enviada difere da anterior.\n");
    }
    else if (strstr(sv_protocol, "NOK")) {
        printf("Erro: NOK (Not Ongoing) - A tentativa está fora de contexto. O jogador não possui um jogo em andamento.\n");
    }
    else if (strstr(sv_protocol, "ENT")) {
        sscanf(sv_protocol, "RTR ENT %c %c %c %c", &cor1, &cor2, &cor3, &cor4);
        printf("Erro: ENT (Exceeded Attempts) - Nenhuma tentativa restante. A chave secreta foi revelada, enviando o código de cores: %c %c %c %c.\n", cor1, cor2, cor3, cor4);
    }
    else if (strstr(sv_protocol, "ETM")) {
        sscanf(sv_protocol, "RTR ETM %c %c %c %c", &cor1, &cor2, &cor3, &cor4);
        printf("Erro: ETM (Exceeded Time) - O tempo máximo de jogo foi excedido. A chave secreta foi revelada, enviando o código de cores: %c %c %c %c.\n", cor1, cor2, cor3, cor4);
    }
    else if (strstr(sv_protocol, "ERR")) {
        printf("Erro: ERR (Syntax Error) - A mensagem de tentativa está com a sintaxe incorreta, o PLID é inválido ou o código de cor C1 C2 C3 C4 não é válido.\n");
    }
    
    else {
        printf("Erro desconhecido: Não foi identificado um código de erro válido na string.\n");
        printf("O erro em questão: %s\n", sv_protocol);
    }
}
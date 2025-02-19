#include "gameEngine.h"


void try_function(Tentativa* tentativa_atual, char* solucao) {       

    int usados_solucao[4]   = {0};      // 
    int usados_tentativa[4] = {0};      // pecas que ja foram usadas e/ou bloqueadas por estarem certas

    for (int i = 0; i < 4; i++) {
        if (tentativa_atual->sequencia[i] == solucao[i]) {
            tentativa_atual->nB++;
            usados_solucao[i] = 1;
            usados_tentativa[i] = 1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (!usados_tentativa[i]) {
            for (int j = 0; j < 4; j++) {
                if (!usados_solucao[j] && tentativa_atual->sequencia[i] == solucao[j]) {
                    tentativa_atual->nW++;
                    usados_solucao[j] = 1;
                    usados_tentativa[i] = 1;
                    break;
                }
            }
        }
    }
    tentativa_atual->num_tentativa++;
}


void gerar_solucao(char* solucao) {
    int i;
    srand(time(NULL)); // criar seed

    for (i = 0; i < 4; i++) {
        int indice = rand() % NUM_CORES; // escolher índice aleatório
        solucao[i] = CORES[indice];     // selecionar a cor correspondente
    }
    solucao[i] = '\0';
}

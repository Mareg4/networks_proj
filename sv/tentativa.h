// tentativa.h
#ifndef TENTATIVA_H
#define TENTATIVA_H

typedef struct {
    char sequencia[5];         // Sequência proposta pelo jogador 5 posicoes a considerar o \0
    int nB;                    // Número de peças certas no lugar certo
    int nW;                    // Número de peças certas no lugar errado
    int num_tentativa;         // Número da tentativa atual
} Tentativa;


#endif // TENTATIVA_H

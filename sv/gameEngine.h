#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "constants.h"
#include "tentativa.h"


/**
 * faz o try (verifica se a solucao do player esta correta e retorna o suposto)
 */
void try_function(Tentativa* tentativa_atual, char* solucao);

/**
 * funcao que gera e retorna uma funcao aleatoria
 */
void gerar_solucao(char* solucao);

#endif // GAME_ENGINE_H
